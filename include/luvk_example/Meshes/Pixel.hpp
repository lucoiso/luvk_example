// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk
{
    class SwapChain;
}

namespace luvk_example
{
    class Pixel : public luvk::Mesh
    {
        std::uint32_t             m_FrameIndex{0};
        std::vector<InstanceInfo> m_LocalInstances{};

    public:
        Pixel(const std::shared_ptr<luvk::Device>&    Device,
              const std::shared_ptr<luvk::SwapChain>& Swap,
              const std::shared_ptr<luvk::Memory>&    Memory);

        void AddInstance(const glm::vec2& Position);
        void Render(VkCommandBuffer CommandBuffer, std::uint32_t CurrentFrame) const override;
    };
}
