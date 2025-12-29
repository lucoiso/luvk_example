/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "UserInterface/Meshes/Pixel.hpp"
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Material.hpp>
#include <luvk/Resources/Pipeline.hpp>

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

Pixel::Pixel(luvk::Device* Device, const luvk::SwapChain* Swap, luvk::Memory* Memory)
    : Mesh(Device, Memory)
{
    constexpr std::array PixVertices = {-0.01f, 0.01f, 0.01f, 0.01f, -0.01f, -0.01f, 0.01f, -0.01f};

    Mesh::UploadVertices(std::as_bytes(std::span{PixVertices}), 4);

    m_InstanceBuffer        = std::make_shared<luvk::Buffer>(Device, Memory);
    const auto     Pipeline = std::make_shared<luvk::Pipeline>(Device);
    const VkFormat Format   = Swap->GetFormat();

    std::vector<VkVertexInputBindingDescription> Bindings = {{0, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX},
                                                             {1, sizeof(PixelInstanceInfo), VK_VERTEX_INPUT_RATE_INSTANCE}};

    std::vector<VkVertexInputAttributeDescription> Attributes = {{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                                                 {1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(PixelInstanceInfo, XForm.Position)},
                                                                 {2, 1, VK_FORMAT_R32_SFLOAT, offsetof(PixelInstanceInfo, XForm.Rotation)},
                                                                 {3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(PixelInstanceInfo, Color)}};

    Pipeline->CreateGraphicsPipeline({.VertexShader     = luvk::CompileShader(g_PixelVertexShader),
                                      .FragmentShader   = luvk::CompileShader(g_PixelFragmentShader),
                                      .ColorFormats     = std::span{&Format, 1},
                                      .Topology         = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                                      .CullMode         = VK_CULL_MODE_NONE,
                                      .VertexBindings   = Bindings,
                                      .VertexAttributes = Attributes});

    const auto Mat = std::make_shared<luvk::Material>();
    Mat->SetPipeline(Pipeline);
    SetMaterial(Mat);
}

void Pixel::AddInstance(const glm::vec2& Position)
{
    PixelInstanceInfo Inst{};
    Inst.XForm.Position[0] = Position.x;
    Inst.XForm.Position[1] = Position.y;
    Inst.XForm.Rotation[0] = 0.0f;
    Inst.Color             = {1.0f, 1.0f, 1.0f, 1.0f};
    m_LocalInstances.push_back(Inst);
}

void Pixel::Render(VkCommandBuffer CommandBuffer) const
{
    if (m_LocalInstances.empty()) return;

    if (m_InstanceBuffer->GetSize() < m_LocalInstances.size() * sizeof(PixelInstanceInfo))
    {
        m_InstanceBuffer->CreateBuffer({.Size               = m_LocalInstances.size() * sizeof(PixelInstanceInfo) * 2,
                                        .Usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                        .MemoryUsage        = VMA_MEMORY_USAGE_CPU_TO_GPU,
                                        .PersistentlyMapped = true});
    }

    m_InstanceBuffer->Upload(std::as_bytes(std::span{m_LocalInstances}));
    m_Material->Bind(CommandBuffer);

    const VkBuffer         Vtx       = m_VertexBuffer->GetHandle();
    const VkBuffer         Inst      = m_InstanceBuffer->GetHandle();
    constexpr VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &Vtx, Offsets);
    vkCmdBindVertexBuffers(CommandBuffer, 1, 1, &Inst, Offsets);

    vkCmdDraw(CommandBuffer, 4, static_cast<std::uint32_t>(m_LocalInstances.size()), 0, 0);
}
