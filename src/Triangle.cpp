// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Triangle.hpp"
#include <random>
#include <glm/gtc/constants.hpp>

using namespace luvk_example;

luvk_example::Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry, const std::size_t Index)
    : m_Registry(std::move(Registry)),
      m_Index(Index),
      m_Mesh(m_Registry, m_Index) {}

void luvk_example::Triangle::AddInstance(const glm::vec2 Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    luvk::MeshRegistry::InstanceInfo Instance{
        .XForm = {.Position = {Position.x, Position.y, 0.F}, .Rotation = std::array{0.F, 0.F, Distribution(Generator) * glm::two_pi<float>()}},
        .Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F}};

    m_Instances.push_back(std::move(Instance));

    if (m_Registry)
    {
        m_Registry->UpdateInstances(m_Index, m_Instances);
    }
}

luvk::Mesh& luvk_example::Triangle::GetMesh() noexcept
{
    return m_Mesh;
}
