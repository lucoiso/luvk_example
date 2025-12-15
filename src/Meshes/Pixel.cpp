// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Pixel.hpp"
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Array.hpp>
#include <luvk/Types/Material.hpp>

using namespace luvk_example;

constexpr auto g_VertexShader = R"(
struct VSInput {
	float2 inPos : POSITION;
	float2 offset : TEXCOORD0;
	float angle : TEXCOORD1;
	float4 instColor : COLOR0;
};

struct VSOutput {
	float4 vColor : COLOR;
	float4 Pos : SV_Position;
};

[shader("vertex")]
VSOutput main(VSInput input)
{
	VSOutput output;
	float2x2 R = float2x2(cos(input.angle), -sin(input.angle), sin(input.angle), cos(input.angle));
	float2 pos = mul(R, input.inPos) + input.offset;
	output.Pos = float4(pos, 0.0f, 1.0f);
	output.vColor = input.instColor;
	return output;
}
)";

constexpr auto g_FragmentShader = R"(
struct VSOutput {
	float4 vColor : COLOR;
};

[shader("fragment")]
float4 main(VSOutput input) : SV_Target
{
	return input.vColor;
}
)";

constexpr luvk::Array<glm::vec2, 13> g_PixVertices{{
    glm::vec2{0.f, 0.f},
    glm::vec2{0.01f, 0.f},
    glm::vec2{0.00866f, 0.005f},
    glm::vec2{0.005f, 0.00866f},
    glm::vec2{0.f, 0.01f},
    glm::vec2{-0.005f, 0.00866f},
    glm::vec2{-0.00866f, 0.005f},
    glm::vec2{-0.01f, 0.f},
    glm::vec2{-0.00866f, -0.005f},
    glm::vec2{-0.005f, -0.00866f},
    glm::vec2{0.f, -0.01f},
    glm::vec2{0.005f, -0.00866f},
    glm::vec2{0.00866f, -0.005f}
}};

constexpr luvk::Array<std::uint16_t, 36> g_PixIndices{{
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

constexpr luvk::Array g_Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                 VkVertexInputBindingDescription{1, sizeof(luvk::Mesh::InstanceInfo), VK_VERTEX_INPUT_RATE_INSTANCE}};

constexpr luvk::Array g_Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
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
                                      .ColorFormats = luvk::Array{Swap->GetCreationArguments().Format},
                                      .RenderPass = Swap->GetRenderPass(),
                                      .VertexShader = luvk::CompileShader(g_VertexShader),
                                      .FragmentShader = luvk::CompileShader(g_FragmentShader),
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
    Inst.XForm.Position.at(0) = Position.x;
    Inst.XForm.Position.at(1) = Position.y;
    Inst.XForm.Rotation.at(2) = 0.F;
    Inst.Color                = {1.F, 1.F, 1.F, 1.F};

    m_LocalInstances.push_back(Inst);
    UpdateInstances(std::as_bytes(std::span{m_LocalInstances}), static_cast<std::uint32_t>(std::size(m_LocalInstances)));
}
