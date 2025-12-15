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
    class Pipeline;
    class Buffer;
    class DescriptorPool;
    class SwapChain;
} // namespace luvk

namespace luvk_example
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
        std::shared_ptr<luvk::Pipeline> m_ComputePipeline{};
        std::shared_ptr<luvk::Buffer>   m_ParticleBuffer{};
        std::shared_ptr<luvk::Buffer>   m_ComputeUBO{};
        std::shared_ptr<luvk::Material> m_GraphicsMat{};
        std::shared_ptr<luvk::Material> m_ComputeMat{};
        std::vector<Particle>           m_Particles{};

    public:
        Triangle(const std::shared_ptr<luvk::Device>&         Device,
                 const std::shared_ptr<luvk::SwapChain>&      Swap,
                 const std::shared_ptr<luvk::Memory>&         Memory,
                 const std::shared_ptr<luvk::DescriptorPool>& Pool);

        void AddInstance(glm::vec2 const& Position);

        void Compute(const VkCommandBuffer& Cmd) const;
        void Draw(const VkCommandBuffer& Cmd, std::span<const std::byte> PushConstants) const override;
    };
} // namespace luvk_example
