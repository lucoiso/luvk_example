// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk
{
    class SwapChain;
} // namespace luvk

namespace luvk_example
{
    class Pixel : public luvk::Mesh
    {
        std::vector<luvk::Mesh::InstanceInfo> m_LocalInstances{};

    public:
        Pixel(const std::shared_ptr<luvk::Device>&    Device,
              const std::shared_ptr<luvk::SwapChain>& Swap,
              const std::shared_ptr<luvk::Memory>&    Memory);

        void AddInstance(const glm::vec2& Position);
    };
} // namespace luvk_example
