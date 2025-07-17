#pragma once

#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/Image.hpp>
#include <luvk/Core/Memory.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Sampler.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example {
/** Mesh usada para renderizar ImGui via luvk */
class ImGuiMesh {
  std::shared_ptr<luvk::MeshRegistry> m_Registry{};
  std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
  std::shared_ptr<luvk::DescriptorSet> m_FontSet{};
  std::shared_ptr<luvk::Image> m_FontImage{};
  std::shared_ptr<luvk::Sampler> m_FontSampler{};
  std::shared_ptr<luvk::Pipeline> m_Pipeline{};
  std::shared_ptr<luvk::Buffer> m_VtxBuffer{};
  std::shared_ptr<luvk::Buffer> m_IdxBuffer{};
  std::shared_ptr<luvk::Device> m_Device{};
  std::shared_ptr<luvk::SwapChain> m_SwapChain{};
  std::shared_ptr<luvk::Memory> m_Memory{};
  std::size_t m_Index{};
  luvk::Mesh m_Mesh{};
  std::size_t m_VtxBufferSize{0};
  std::size_t m_IdxBufferSize{0};
  std::vector<ImDrawVert> m_Vertices{};
  std::vector<ImDrawIdx> m_Indices{};

public:
  /**
   * @brief Construtor
   * @param Registry Gerenciador de malhas
   * @param Device   Dispositivo Vulkan
   * @param Swap     Swapchain
   * @param Memory   Allocador de memoria
   */
  ImGuiMesh(std::shared_ptr<luvk::MeshRegistry> Registry,
            std::shared_ptr<luvk::Device> Device,
            std::shared_ptr<luvk::SwapChain> Swap,
            std::shared_ptr<luvk::Memory> Memory);

  /** Prepara novo frame de ImGui */
  void NewFrame() const;

  /** Executa a renderizacao do draw data */
  void Render(const VkCommandBuffer &Cmd);

  /** Mesh resultante para registro na renderer */
  [[nodiscard]] luvk::Mesh &GetMesh() noexcept;
};
} // namespace luvk_example
