// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Pixel.hpp"
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>

using namespace luvk_example;

constexpr auto g_VertexShader = R"(
    #version 450

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
    }
)";

constexpr auto g_FragmentShader = R"(
    #version 450

    layout(location = 0) in vec4 vColor;
    layout(location = 0) out vec4 outColor;

    void main()
    {
        outColor = vColor;
    }
)";

constexpr std::array<glm::vec2, 13> g_PixVertices{{
    {0.f, 0.f},
    {0.01f, 0.f},
    {0.00866f, 0.005f},
    {0.005f, 0.00866f},
    {0.f, 0.01f},
    {-0.005f, 0.00866f},
    {-0.00866f, 0.005f},
    {-0.01f, 0.f},
    {-0.00866f, -0.005f},
    {-0.005f, -0.00866f},
    {0.f, -0.01f},
    {0.005f, -0.00866f},
    {0.00866f, -0.005f}
}};

constexpr std::array<std::uint16_t, 36> g_PixIndices{{
    0,
    1,
    2,
    0,
    2,
    3,
    0,
    3,
    4,
    0,
    4,
    5,
    0,
    5,
    6,
    0,
    6,
    7,
    0,
    7,
    8,
    0,
    8,
    9,
    0,
    9,
    10,
    0,
    10,
    11,
    0,
    11,
    12,
    0,
    12,
    1
}};

constexpr std::array g_Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                VkVertexInputBindingDescription{1, sizeof(luvk::Mesh::InstanceInfo), VK_VERTEX_INPUT_RATE_INSTANCE}};

constexpr std::array g_Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                  VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, XForm.Position)},
                                  VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, XForm.Rotation) + sizeof(float) * 2},
                                  VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, Color)}};

Pixel::Pixel(const std::shared_ptr<luvk::Device>&    Device,
             const std::shared_ptr<luvk::SwapChain>& Swap,
             const std::shared_ptr<luvk::Memory>&    Memory)
    : Mesh(Device, Memory)
{
    const auto Pipeline = std::make_shared<luvk::Pipeline>(Device);

    Pipeline->CreateGraphicsPipeline({.Extent = Swap->GetExtent(),
                                      .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                      .RenderPass = Swap->GetRenderPass(),
                                      .VertexShader = luvk::CompileGLSLToSPIRV(g_VertexShader, EShLangVertex),
                                      .FragmentShader = luvk::CompileGLSLToSPIRV(g_FragmentShader, EShLangFragment),
                                      .Bindings = g_Bindings,
                                      .Attributes = g_Attributes,
                                      .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                      .CullMode = VK_CULL_MODE_NONE});

    const auto Mat = std::make_shared<luvk::Material>();
    Mat->Initialize(Device, nullptr, Memory, Pipeline);
    SetMaterial(Mat);

    UploadVertices(std::as_bytes(std::span{g_PixVertices}), std::size(g_PixVertices));
    UploadIndices(std::span{g_PixIndices});
}

void Pixel::AddInstance(const glm::vec2& Position)
{
    luvk::Mesh::InstanceInfo Inst{};
    Inst.XForm.Position[0] = Position.x;
    Inst.XForm.Position[1] = Position.y;
    Inst.XForm.Rotation[2] = 0.F;
    Inst.Color             = {1.F, 1.F, 1.F, 1.F};

    m_LocalInstances.push_back(Inst);
    UpdateInstances(std::as_bytes(std::span{m_LocalInstances}), static_cast<std::uint32_t>(std::size(m_LocalInstances)));
}
