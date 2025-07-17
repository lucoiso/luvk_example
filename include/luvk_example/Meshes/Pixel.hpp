// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example {
/** Mesh para particulas de um pixel */
class Pixel {
  std::shared_ptr<luvk::MeshRegistry> m_Registry{};
  std::shared_ptr<luvk::Pipeline> m_Pipeline{};
  std::size_t m_Index{};
  luvk::Mesh m_Mesh{};
  std::vector<luvk::MeshRegistry::InstanceInfo> m_Instances{};

public:
  /** Construtor */
  Pixel(std::shared_ptr<luvk::MeshRegistry> Registry,
        const std::shared_ptr<luvk::Device> &Device,
        const std::shared_ptr<luvk::SwapChain> &Swap);

  /** Adiciona uma instancia */
  void AddInstance(glm::vec2 Position);

  /** Mesh para registro */
  [[nodiscard]] luvk::Mesh &GetMesh() noexcept;
};
} // namespace luvk_example
