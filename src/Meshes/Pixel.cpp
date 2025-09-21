// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Pixel.hpp"
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr auto TriVertSrc = R"(#version 450

                                   layout(location = 0) in vec2 inPos;
                                   layout(location = 1) in vec2 offset;
                                   layout(location = 2) in float angle;
                                   layout(location = 3) in vec4 instColor;
                                   layout(location = 0) out vec4 vColor;

                                   void main()
                                   {
                                       mat2 R = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
                                       vec2 pos = R * inPos + offset;
                                       gl_Position = vec4(pos, 0.0, 1.0);
                                       vColor = instColor;
                                   })";

    constexpr auto TriFragSrc = R"(#version 450

                                   layout(location = 0) in vec4 vColor;
                                   layout(location = 0) out vec4 outColor;

                                   void main()
                                   {
                                       outColor = vColor;
                                   })";

    constexpr std::array<glm::vec2, 12> PixelVertices{{{0.01f, 0.F},
                                                       {0.00866f, 0.005f},
                                                       {0.005f, 0.00866f},
                                                       {0.F, 0.01f},
                                                       {-0.005f, 0.00866f},
                                                       {-0.00866f, 0.005f},
                                                       {-0.01f, 0.F},
                                                       {-0.00866f, -0.005f},
                                                       {-0.005f, -0.00866f},
                                                       {0.F, -0.01f},
                                                       {0.005f, -0.00866f},
                                                       {0.00866f, -0.005f}}};

    constexpr std::array<std::uint16_t, 30> PixelIndices{{0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 9, 0, 9, 10, 0, 10, 11}};
} // namespace

Pixel::Pixel(std::shared_ptr<luvk::MeshRegistry> Registry,
             const std::shared_ptr<luvk::Device>& Device,
             const std::shared_ptr<luvk::SwapChain>& Swap)
    : m_Registry(std::move(Registry)),
      m_Pipeline(std::make_shared<luvk::Pipeline>(Device))
{
    auto TriVert = luvk::CompileGLSLToSPIRV(TriVertSrc, EShLangVertex);
    auto TriFrag = luvk::CompileGLSLToSPIRV(TriFragSrc, EShLangFragment);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->GetCreationArguments().Format};

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(luvk::MeshInstance), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::MeshInstance, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::MeshInstance, Angle)},
                              VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::MeshInstance, Color)}};

    m_Pipeline->CreateGraphicsPipeline({.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = Swap->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = TriVert,
                                        .FragmentShader = TriFrag,
                                        .SetLayouts = {},
                                        .Bindings = std::array{TriBindings.at(0), TriBindings.at(1)},
                                        .Attributes = std::array{TriAttrs.at(0), TriAttrs.at(1), TriAttrs.at(2), TriAttrs.at(3)},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{PixelVertices}),
                                       std::as_bytes(std::span{PixelIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       {},
                                       m_Pipeline);

    m_Mesh = std::make_shared<luvk::Mesh>(m_Registry, m_Index);
}

void Pixel::AddInstance(glm::vec2 const& Position)
{
    luvk::MeshRegistry::InstanceInfo Instance{.XForm = {.Position = {Position.x, Position.y, 0.F}}, .Color = {1.F, 1.F, 1.F, 1.F}};
    m_Instances.push_back(std::move(Instance));

    m_Registry->UpdateInstances(m_Index, m_Instances);
}
