// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Triangle.hpp"
#include <random>
#include <glm/gtc/constants.hpp>
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
                                   void main() {
                                       mat2 R = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
                                       vec2 pos = R * inPos + offset;
                                       gl_Position = vec4(pos, 0.0, 1.0);
                                       vColor = instColor;
                                   })";

    constexpr auto TriFragSrc = R"(#version 450
                                   layout(location = 0) in vec4 vColor;
                                   layout(location = 0) out vec4 outColor;
                                   void main() {
                                       outColor = vColor;
                                   })";

    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    { 0.05f, -0.05f},
                                                    { 0.0f,   0.1f}}};
    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};
} // namespace

luvk_example::Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                                 std::shared_ptr<luvk::Device> Device,
                                 std::shared_ptr<luvk::SwapChain> Swap)
    : m_Registry(std::move(Registry)),
      m_Pipeline(std::make_shared<luvk::Pipeline>())
{
    auto TriVert = luvk::CompileGLSLToSPIRV(TriVertSrc, EShLangVertex);
    auto TriFrag = luvk::CompileGLSLToSPIRV(TriFragSrc, EShLangFragment);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->m_Arguments.Format};

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(luvk::MeshInstance), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::MeshInstance, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::MeshInstance, Angle)},
                              VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::MeshInstance, Color)}};

    m_Pipeline->CreateGraphicsPipeline(Device,
                                        {.Extent = Extent,
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

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{TriVertices}),
                                       std::as_bytes(std::span{TriIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       {},
                                       m_Pipeline,
                                       Device);

    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void luvk_example::Triangle::AddInstance(const glm::vec2 Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    luvk::MeshRegistry::InstanceInfo Instance{
        .XForm = {.Position = {Position.x, Position.y, 0.F}, .Rotation = std::array{0.F, 0.F, Distribution(Generator) * glm::two_pi<float>()}},
        .Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F}};

    m_Instances.push_back(std::move(Instance));

    if (m_Registry)
    {
        m_Registry->UpdateInstances(m_Index, m_Instances);
    }
}

luvk::Mesh& luvk_example::Triangle::GetMesh() noexcept
{
    return m_Mesh;
}
