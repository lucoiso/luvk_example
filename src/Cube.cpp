// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr auto CubeVertSrc = R"(#version 450

                                    layout(push_constant) uniform Push
                                    {
                                        mat4 mvp;
                                        vec4 color;
                                    } pc;

                                    layout(location = 0) in vec3 inPos;
                                    layout(location = 0) out vec4 vColor;

                                    void main()
                                    {
                                        gl_Position = pc.mvp * vec4(inPos, 1.0);
                                        vColor = pc.color;
                                    })";

    constexpr auto CubeFragSrc = R"(#version 450

                                    layout(location = 0) in vec4 vColor;
                                    layout(location = 0) out vec4 outColor;

                                    void main()
                                    {
                                        outColor = vColor;
                                    })";

    constexpr std::array<glm::vec3, 8> CubeVertices{{{-0.5f, -0.5f, -0.5f},
                                                     {0.5f, -0.5f, -0.5f},
                                                     {0.5f, 0.5f, -0.5f},
                                                     {-0.5f, 0.5f, -0.5f},
                                                     {-0.5f, -0.5f, 0.5f},
                                                     {0.5f, -0.5f, 0.5f},
                                                     {0.5f, 0.5f, 0.5f},
                                                     {-0.5f, 0.5f, 0.5f}}};

    constexpr std::array<std::uint16_t, 36> CubeIndices{{0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 1, 5, 5, 4, 0, 2, 3, 7, 7, 6, 2, 1, 2, 6, 6, 5, 1, 3, 0, 4, 4, 7, 3}};
} // namespace

luvk_example::Cube::Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
                         const std::shared_ptr<luvk::Device>& Device,
                         const std::shared_ptr<luvk::SwapChain>& Swap,
                         const std::shared_ptr<luvk::Memory>& Memory)
    : m_Registry(std::move(Registry)),
      m_Pipeline(std::make_shared<luvk::Pipeline>()),
      m_UBO(std::make_shared<luvk::Buffer>())
{
    auto CubeVert = luvk::CompileGLSLToSPIRV(CubeVertSrc, EShLangVertex);
    auto CubeFrag = luvk::CompileGLSLToSPIRV(CubeFragSrc, EShLangFragment);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->m_Arguments.Format};

    constexpr VkVertexInputBindingDescription CubeBinding{0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX};
    constexpr VkVertexInputAttributeDescription CubeAttr{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
    constexpr VkPushConstantRange CubePC{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::mat4) + sizeof(glm::vec4)};

    m_Pipeline->CreateGraphicsPipeline(Device,
                                       {.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = Swap->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = CubeVert,
                                        .FragmentShader = CubeFrag,
                                        .SetLayouts = {},
                                        .Bindings = std::array{CubeBinding},
                                        .Attributes = std::array{CubeAttr},
                                        .PushConstants = std::array{CubePC},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST});

    m_UBO->CreateBuffer(Memory,
                        {.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, .Size = sizeof(glm::mat4) + sizeof(glm::vec4), .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{CubeVertices}),
                                       std::as_bytes(std::span{CubeIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       m_UBO,
                                       {},
                                       m_Pipeline,
                                       Device);

    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void luvk_example::Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static float Elapsed = 0.F;
    Elapsed += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    const glm::mat4 Mvp = Proj * View * Model;

    const glm::vec4 Color{0.5F + 0.5F * std::sin(Elapsed), 0.5F + 0.5F * std::sin(Elapsed + 2.094F), 0.5F + 0.5F * std::sin(Elapsed + 4.188F), 1.f};

    struct
    {
        glm::mat4 Mvp;
        glm::vec4 Color;
    } Pc{Mvp, Color};

    m_Registry->UpdateUniform(m_Index, std::as_bytes(std::span{&Pc, 1}));
}

luvk::Mesh& luvk_example::Cube::GetMesh() noexcept
{
    return m_Mesh;
}
