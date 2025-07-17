// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include <luvk/Core/Buffer.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example {
/** Mesh de cubo simples */
class Cube {
  std::shared_ptr<luvk::MeshRegistry> m_Registry{};
  std::shared_ptr<luvk::Pipeline> m_Pipeline{};
  std::shared_ptr<luvk::Buffer> m_UBO{};
  std::size_t m_Index{};
  luvk::Mesh m_Mesh{};

public:
  /** Construtor do cubo */
  Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
       const std::shared_ptr<luvk::Device> &Device,
       const std::shared_ptr<luvk::SwapChain> &Swap,
       const std::shared_ptr<luvk::Memory> &Memory);

  /** Atualiza uniformes */
  void Update(float DeltaTime, glm::mat4 const &View,
              glm::mat4 const &Proj) const;

  /** Mesh para registro */
  [[nodiscard]] luvk::Mesh &GetMesh() noexcept;
};
} // namespace luvk_example
