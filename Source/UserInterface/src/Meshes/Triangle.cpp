/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "UserInterface/Meshes/Triangle.hpp"
#include <random>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Material.hpp>
#include <luvk/Resources/Pipeline.hpp>

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

Triangle::Triangle(luvk::Device* Device, const luvk::SwapChain* Swap, luvk::Memory* Memory, luvk::DescriptorPool* Pool)
    : Mesh(Device, Memory),
      m_ComputePipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_ParticleBuffer(std::make_shared<luvk::Buffer>(Device, Memory)),
      m_GraphicsMat(std::make_shared<luvk::Material>()),
      m_ComputeMat(std::make_shared<luvk::Material>())
{
    constexpr std::array Vertices = {0.0f, -0.05f, 0.05f, 0.05f, -0.05f, 0.05f};
    m_VertexCount                 = 3;
    m_VertexBuffer                = std::make_shared<luvk::Buffer>(Device, Memory);
    m_VertexBuffer->CreateBuffer({.Size = sizeof(Vertices), .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT});
    m_VertexBuffer->Upload(std::as_bytes(std::span{Vertices}));

    m_ParticleBuffer->CreateBuffer({.Size = sizeof(Particle) * 2048,
                                    .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU,
                                    .PersistentlyMapped = true,
                                    .Name = "ParticleBuffer"});

    constexpr VkDescriptorSetLayoutBinding DescriptorBinding{0,
                                                             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                             1,
                                                             VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
                                                             nullptr};
    const auto Set = std::make_shared<luvk::DescriptorSet>(Device, Pool);
    Set->CreateLayout(std::span{&DescriptorBinding, 1});
    Set->Allocate();
    Set->UpdateStorageBuffer(0, m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize());

    auto SetLayout = Set->GetLayout();

    const auto GraphicsPipeline = std::make_shared<luvk::Pipeline>(Device);
    VkFormat   Format           = Swap->GetFormat();

    std::vector<VkVertexInputBindingDescription> Bindings = {{0, sizeof(float) * 2, VK_VERTEX_INPUT_RATE_VERTEX},
                                                             {1, sizeof(Particle), VK_VERTEX_INPUT_RATE_INSTANCE}};

    std::vector<VkVertexInputAttributeDescription> Attributes = {{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                                                 {1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, Offset)},
                                                                 {2, 1, VK_FORMAT_R32_SFLOAT, offsetof(Particle, Angle)},
                                                                 {3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, Color)}};

    GraphicsPipeline->CreateGraphicsPipeline({.VertexShader     = luvk::CompileShader(g_TriangleVertexShader),
                                              .FragmentShader   = luvk::CompileShader(g_TriangleFragmentShader),
                                              .ColorFormats     = std::span{&Format, 1},
                                              .Topology         = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                              .CullMode         = VK_CULL_MODE_NONE,
                                              .SetLayouts       = std::span{&SetLayout, 1},
                                              .VertexBindings   = Bindings,
                                              .VertexAttributes = Attributes});

    m_ComputePipeline->CreateComputePipeline({.ComputeShader = luvk::CompileShader(g_TriangleComputeShader),
                                              .SetLayouts    = std::span{&SetLayout, 1},
                                              .PushConstants = std::span{&g_ConstantRange, 1}});

    m_GraphicsMat->SetPipeline(GraphicsPipeline);
    m_GraphicsMat->SetDescriptorSet(0, Set);

    m_ComputeMat->SetPipeline(m_ComputePipeline);
    m_ComputeMat->SetDescriptorSet(0, Set);
}

void Triangle::Compute(const VkCommandBuffer Cmd) const
{
    if (m_Particles.empty())
    {
        return;
    }

    m_ComputeMat->Bind(Cmd);

    const VkDescriptorSet ComputeSet = m_ComputeMat->GetDescriptorSet(0)->GetHandle();
    vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->GetLayout(), 0, 1, &ComputeSet, 0, nullptr);

    vkCmdPushConstants(Cmd, m_ComputePipeline->GetLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &m_DeltaTime);

    const auto DispatchX = static_cast<std::uint32_t>(std::ceil(static_cast<float>(m_Particles.size()) / 64.0f));
    vkCmdDispatch(Cmd, DispatchX, 1, 1);
}

void Triangle::Tick(const float DeltaTime)
{
    m_DeltaTime = DeltaTime;
}

void Triangle::Render(const VkCommandBuffer Cmd) const
{
    if (m_Particles.empty())
    {
        return;
    }

    m_GraphicsMat->Bind(Cmd);

    const VkBuffer         VtxBuf    = m_VertexBuffer->GetHandle();
    const VkBuffer         InstBuf   = m_ParticleBuffer->GetHandle();
    constexpr VkDeviceSize Offsets[] = {0};

    vkCmdBindVertexBuffers(Cmd, 0, 1, &VtxBuf, Offsets);
    vkCmdBindVertexBuffers(Cmd, 1, 1, &InstBuf, Offsets);

    vkCmdDraw(Cmd, 3, static_cast<std::uint32_t>(m_Particles.size()), 0, 0);
}

void Triangle::AddInstance([[maybe_unused]] glm::vec2 const& Position)
{
    std::random_device             RandomDevice;
    std::mt19937                   RandomGenerator(RandomDevice());
    std::uniform_real_distribution Distribution(0.0f, 1.0f);

    m_Particles.push_back(Particle{.Offset   = Position,
                                   .Angle    = Distribution(RandomGenerator) * 3.14f * 2.0f,
                                   .Color    = glm::vec4(Distribution(RandomGenerator), Distribution(RandomGenerator), Distribution(RandomGenerator), 1.0f),
                                   .Velocity = glm::vec2(0.0f)});

    m_ParticleBuffer->Upload(std::as_bytes(std::span{m_Particles}));
}
