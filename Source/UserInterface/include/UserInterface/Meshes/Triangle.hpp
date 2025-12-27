/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Types/Mesh.hpp>

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
        float                           m_DeltaTime{0.f};
        std::shared_ptr<luvk::Pipeline> m_ComputePipeline{};
        std::shared_ptr<luvk::Buffer>   m_ParticleBuffer{};
        std::shared_ptr<luvk::Material> m_GraphicsMat{};
        std::shared_ptr<luvk::Material> m_ComputeMat{};
        std::vector<Particle>           m_Particles{};

    public:
        Triangle(const std::shared_ptr<luvk::Device>&         Device,
                 const std::shared_ptr<luvk::SwapChain>&      Swap,
                 const std::shared_ptr<luvk::Memory>&         Memory,
                 const std::shared_ptr<luvk::DescriptorPool>& Pool);

        void AddInstance(glm::vec2 const& Position);

        void Compute(VkCommandBuffer Cmd) const;

        void Tick(float DeltaTime) override;
        void Render(VkCommandBuffer Cmd, std::uint32_t CurrentFrame) const override;
    };
}
