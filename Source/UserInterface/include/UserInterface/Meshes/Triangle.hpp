/*
* Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Resources/Mesh.hpp>

namespace luvk
{
    class Pipeline;
    class Buffer;
    class DescriptorPool;
    class SwapChain;
}

namespace UserInterface
{
    struct alignas(16) Particle
    {
        glm::vec2 Offset;
        float     Angle;
        float     _pad1;
        glm::vec4 Color;
        glm::vec2 Velocity;
        glm::vec2 _pad2;
    };

    class Triangle : public luvk::Mesh
    {
        float                           m_DeltaTime{0.F};
        std::shared_ptr<luvk::Pipeline> m_ComputePipeline{};
        std::shared_ptr<luvk::Buffer>   m_ParticleBuffer{};
        std::shared_ptr<luvk::Material> m_GraphicsMat{};
        std::shared_ptr<luvk::Material> m_ComputeMat{};
        std::vector<Particle>           m_Particles{};

    public:
        Triangle(luvk::Device* Device, const luvk::SwapChain* Swap, luvk::Memory* Memory, luvk::DescriptorPool* Pool);

        void AddInstance(glm::vec2 const& Position);
        void Compute(VkCommandBuffer Cmd) const;
        void Tick(float DeltaTime);
        void Render(VkCommandBuffer Cmd) const override;
    };
}
