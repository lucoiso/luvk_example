// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "UserInterface/Meshes/Triangle.hpp"
#include <random>
#include <glm/gtc/constants.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>

using namespace UserInterface;

constexpr auto g_TriangleVertexShader = R"(
struct VSInput {
	float2 inPos : POSITION;
	float2 offset : TEXCOORD0;
	float angle : TEXCOORD1;
	float4 color : COLOR0;
};

[shader("vertex")]
void main(
    VSInput input,
    out float4 Pos : SV_Position,
    out float4 vColor : COLOR)
{
	float2x2 R = float2x2(cos(input.angle), -sin(input.angle),
						  sin(input.angle), cos(input.angle));
	float2 pos = mul(R, input.inPos) + input.offset;
	Pos = float4(pos, 0.0f, 1.0f);
	vColor = input.color;
}
)";

constexpr auto g_TriangleFragmentShader = R"(
[shader("fragment")]
float4 main(float4 vColor : COLOR) : SV_Target
{
	return vColor;
}
)";

constexpr auto g_TriangleComputeShader = R"(
struct Particle {
    float2 offset;
    float angle;
    float4 color;
    float2 velocity;
};

struct PC {
    float dt;
};

[[vk::push_constant]] PC pc;
[[vk::binding(0,0)]] RWStructuredBuffer<Particle> PartData;

[shader("compute")]
[numthreads(64, 1, 1)]
void main(uint3 gl_GlobalInvocationID : SV_DispatchThreadID)
{
    uint id = gl_GlobalInvocationID.x;
    PartData[id].angle += pc.dt * 2.0f;
}
)";

constexpr VkPushConstantRange g_ConstantRange{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};

Triangle::Triangle(const std::shared_ptr<luvk::Device>&         Device,
                   const std::shared_ptr<luvk::SwapChain>&      Swap,
                   const std::shared_ptr<luvk::Memory>&         Memory,
                   const std::shared_ptr<luvk::DescriptorPool>& Pool)
    : Mesh(Device, Memory),
      m_ComputePipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_ParticleBuffer(std::make_shared<luvk::Buffer>(Device, Memory)),
      m_GraphicsMat(std::make_shared<luvk::Material>()),
      m_ComputeMat(std::make_shared<luvk::Material>())
{
    constexpr std::array<glm::vec2, 3> Vertices{glm::vec2{-0.05F, -0.05F},
                                                glm::vec2{0.05F, -0.05F},
                                                glm::vec2{0.F, 0.1F}};

    constexpr std::array<std::uint16_t, 3> Indices{{0, 1, 2}};

    constexpr VkDescriptorSetLayoutBinding DescriptorBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
    constexpr VkDescriptorPoolSize         DescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};

    constexpr std::array Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                  VkVertexInputBindingDescription{1, sizeof(Particle), VK_VERTEX_INPUT_RATE_INSTANCE}};

    constexpr std::array Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                    VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, Offset)},
                                    VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(Particle, Angle)},
                                    VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, Color)}};

    const auto Set = std::make_shared<luvk::DescriptorSet>(Device, Pool, Memory);
    Set->CreateLayout({.Bindings = std::array{DescriptorBinding}});
    Set->Allocate();

    const auto GraphicsPipeline = std::make_shared<luvk::Pipeline>(Device);
    GraphicsPipeline->CreateGraphicsPipeline({.Extent = Swap->GetExtent(),
                                              .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                              .RenderPass = Swap->GetRenderPass(),
                                              .Subpass = 0,
                                              .VertexShader = luvk::CompileShader(g_TriangleVertexShader),
                                              .FragmentShader = luvk::CompileShader(g_TriangleFragmentShader),
                                              .SetLayouts = std::array{Set->GetLayout()},
                                              .Bindings = Bindings,
                                              .Attributes = Attributes,
                                              .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                              .CullMode = VK_CULL_MODE_NONE});

    m_ComputePipeline->CreateComputePipeline({.ComputeShader = luvk::CompileShader(g_TriangleComputeShader),
                                              .SetLayouts = std::array{Set->GetLayout()},
                                              .PushConstants = std::array{g_ConstantRange}});

    m_ParticleBuffer->CreateBuffer({.Size = sizeof(Particle) * 1000,
                                    .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                    .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU,
                                    .Name = "Particle VTX"});

    Set->UpdateBuffer(m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    m_GraphicsMat->SetPipeline(GraphicsPipeline);
    m_GraphicsMat->SetDescriptorSet(Set);

    m_ComputeMat->SetPipeline(m_ComputePipeline);
    m_ComputeMat->SetDescriptorSet(Set);

    SetMaterial(m_GraphicsMat);
    UploadVerticesToAll(std::as_bytes(std::span{Vertices}), 3);
    UploadIndicesToAll(std::span{Indices});
}

void Triangle::Compute(const VkCommandBuffer Cmd) const
{
    if (std::empty(m_Particles))
    {
        return;
    }

    m_ComputeMat->Bind(Cmd);
    vkCmdPushConstants(Cmd, m_ComputePipeline->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &m_DeltaTime);
    vkCmdDispatch(Cmd, (static_cast<uint32_t>(std::size(m_Particles)) + 63) / 64, 1, 1);

    const VkBufferMemoryBarrier Barrier{.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                                        .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                                        .dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
                                        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                        .buffer = m_ParticleBuffer->GetHandle(),
                                        .offset = 0,
                                        .size = VK_WHOLE_SIZE};

    vkCmdPipelineBarrier(Cmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &Barrier, 0, nullptr);
}

void Triangle::Tick(const float DeltaTime)
{
    m_DeltaTime = DeltaTime;
}

void Triangle::Render(const VkCommandBuffer Cmd, const std::uint32_t CurrentFrame) const
{
    m_GraphicsMat->Bind(Cmd);

    if (!std::empty(m_VertexBuffers) && m_VertexBuffers[CurrentFrame])
    {
        const std::array<VkBuffer, 2U>         Buffers = {m_VertexBuffers[CurrentFrame]->GetHandle(), m_ParticleBuffer->GetHandle()};
        constexpr std::array<VkDeviceSize, 2U> Offsets = {0, 0};

        vkCmdBindVertexBuffers(Cmd, 0, 2U, std::data(Buffers), std::data(Offsets));
    }

    if (!std::empty(m_IndexBuffers) && m_IndexBuffers[CurrentFrame])
    {
        vkCmdBindIndexBuffer(Cmd, m_IndexBuffers[CurrentFrame]->GetHandle(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(Cmd, m_IndexCount, m_InstanceCount, 0, 0, 0);
    }
}

void Triangle::AddInstance(glm::vec2 const& Position)
{
    static std::mt19937                   Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    m_Particles.push_back(Particle{.Offset = Position,
                                   .Angle = Distribution(Generator) * glm::two_pi<float>(),
                                   .Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F},
                                   .Velocity = {0.F, 0.F}});

    m_ParticleBuffer->Upload(std::as_bytes(std::span{m_Particles}));
    m_InstanceCount = static_cast<uint32_t>(std::size(m_Particles));
}
