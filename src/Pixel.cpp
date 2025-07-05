// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Pixel.hpp"

using namespace luvk_example;

luvk_example::Pixel::Pixel(std::shared_ptr<luvk::MeshRegistry> Registry, const std::size_t Index)
    : m_Registry(std::move(Registry)),
      m_Index(Index),
      m_Mesh(m_Registry, m_Index) {}

void luvk_example::Pixel::AddInstance(const glm::vec2 Position)
{
    luvk::MeshRegistry::InstanceInfo Instance{.XForm = {.Position = {Position.x, Position.y, 0.F}}, .Color = {1.F, 1.F, 1.F, 1.F}};
    m_Instances.push_back(std::move(Instance));

    if (m_Registry)
    {
        m_Registry->UpdateInstances(m_Index, m_Instances);
    }
}

luvk::Mesh& luvk_example::Pixel::GetMesh() noexcept
{
    return m_Mesh;
}
