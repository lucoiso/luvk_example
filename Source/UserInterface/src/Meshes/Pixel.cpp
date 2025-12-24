// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "UserInterface/Meshes/Pixel.hpp"
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>

using namespace UserInterface;

constexpr auto g_PixelVertexShader = R"(
struct VSInput {
	float2 inPos : POSITION;
	float2 offset : TEXCOORD0;
	float angle : TEXCOORD1;
	float4 instColor : COLOR0;
};

[shader("vertex")]
void main(
    VSInput input,
    out float4 Pos : SV_Position,
    out float4 vColor : COLOR)
{
	float2x2 R = float2x2(cos(input.angle), -sin(input.angle), sin(input.angle), cos(input.angle));
	float2 pos = mul(R, input.inPos) + input.offset;
	Pos = float4(pos, 0.0f, 1.0f);
	vColor = input.instColor;
}
)";

constexpr auto g_PixelFragmentShader = R"(
[shader("fragment")]
float4 main(float4 vColor : COLOR) : SV_Target
{
	return vColor;
}
)";

Pixel::Pixel(const std::shared_ptr<luvk::Device>&    Device,
             const std::shared_ptr<luvk::SwapChain>& Swap,
             const std::shared_ptr<luvk::Memory>&    Memory)
    : Mesh(Device, Memory)
{
    constexpr std::array<glm::vec2, 4> PixVertices{{glm::vec2{-0.01f, 0.01f},
                                                    glm::vec2{0.01f, 0.01f},
                                                    glm::vec2{-0.01f, -0.01f},
                                                    glm::vec2{0.01f, -0.01f}}};

    constexpr std::array<std::uint16_t, 6> PixIndices{{0, 2, 1, 2, 3, 1}};

    constexpr std::array Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                  VkVertexInputBindingDescription{1, sizeof(luvk::Mesh::InstanceInfo), VK_VERTEX_INPUT_RATE_INSTANCE}};

    constexpr std::array Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                    VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, XForm.Position)},
                                    VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, XForm.Rotation) + sizeof(float) * 2},
                                    VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::Mesh::InstanceInfo, Color)}};

    const auto Pipeline = std::make_shared<luvk::Pipeline>(Device);

    Pipeline->CreateGraphicsPipeline({.Extent = Swap->GetExtent(),
                                      .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                      .RenderPass = Swap->GetRenderPass(),
                                      .VertexShader = luvk::CompileShader(g_PixelVertexShader),
                                      .FragmentShader = luvk::CompileShader(g_PixelFragmentShader),
                                      .Bindings = Bindings,
                                      .Attributes = Attributes,
                                      .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                      .CullMode = VK_CULL_MODE_NONE});

    const auto Mat = std::make_shared<luvk::Material>();
    Mat->Initialize(Device, nullptr, Memory, Pipeline);
    SetMaterial(Mat);

    UploadVerticesToAll(std::as_bytes(std::span{PixVertices}), std::size(PixVertices));
    UploadIndicesToAll(std::span{PixIndices});
}

void Pixel::AddInstance(const glm::vec2& Position)
{
    InstanceInfo Inst{};
    Inst.XForm.Position.at(0) = Position.x;
    Inst.XForm.Position.at(1) = Position.y;
    Inst.XForm.Rotation.at(2) = 0.F;
    Inst.Color                = {0.5F, 0.5F, 0.5F, 1.F};

    m_LocalInstances.push_back(Inst);
}

void Pixel::Render(const VkCommandBuffer CommandBuffer, const std::uint32_t CurrentFrame) const
{
    auto& Self = const_cast<Pixel&>(*this);
    if (!m_LocalInstances.empty())
    {
        Self.UpdateInstances(std::as_bytes(std::span{m_LocalInstances}), static_cast<std::uint32_t>(m_LocalInstances.size()), CurrentFrame);
    }
    Mesh::Render(CommandBuffer, CurrentFrame);
}
