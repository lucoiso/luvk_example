/*
* Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <glm/glm.hpp>
#include <luvk/Resources/Mesh.hpp>

namespace luvk
{
    class SwapChain;
}

namespace UserInterface
{
    struct PixelInstanceInfo
    {
        luvk::Transform      XForm;
        std::array<float, 4> Color{};
    };

    class Pixel : public luvk::Mesh
    {
        std::vector<PixelInstanceInfo> m_LocalInstances{};
        std::shared_ptr<luvk::Buffer>  m_InstanceBuffer;

    public:
        Pixel(luvk::Device* Device, const luvk::SwapChain* Swap, luvk::Memory* Memory);

        void AddInstance(const glm::vec2& Position);
        void Render(VkCommandBuffer CommandBuffer) const override;
    };
}
