// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include <luvk/Core/Buffer.hpp>
#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/Memory.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example {
/** Triangle particle mesh */
class Triangle {
  struct alignas(16) Particle {
    glm::vec2 Offset{};
    float Angle{0.F};
    float _pad1;
    glm::vec4 Color{1.F, 1.F, 1.F, 1.F};
    glm::vec2 Velocity{};
    glm::vec2 _pad2;
  };

  std::shared_ptr<luvk::MeshRegistry> m_Registry{};
  std::shared_ptr<luvk::Pipeline> m_GraphicsPipeline{};
  std::shared_ptr<luvk::Pipeline> m_ComputePipeline{};
  std::shared_ptr<luvk::Buffer> m_ParticleBuffer{};
  std::shared_ptr<luvk::Buffer> m_ComputeUBO{};
  std::shared_ptr<luvk::DescriptorPool> m_DescriptorPool{};
  std::shared_ptr<luvk::DescriptorSet> m_DescriptorSet{};
  std::shared_ptr<luvk::Device> m_Device{};
  std::size_t m_GraphicsIndex{};
  std::size_t m_ComputeIndex{};
  luvk::Mesh m_Mesh{};
  std::vector<Particle> m_Particles{};

public:
  /** Constructor */
  Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
           const std::shared_ptr<luvk::Device> &Device,
           const std::shared_ptr<luvk::SwapChain> &Swap,
           const std::shared_ptr<luvk::Memory> &Memory);

  /** Updates particles */
  void Update(float DeltaTime) const;

  /** Adds a new particle */
  void AddInstance(glm::vec2 const &Position);

  /** Resulting mesh */
  [[nodiscard]] constexpr luvk::Mesh &GetMesh() noexcept;
};
} // namespace luvk_example
