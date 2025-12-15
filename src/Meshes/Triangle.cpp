// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Triangle.hpp"
#include <array>
#include <random>
#include <glm/gtc/constants.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Material.hpp>

using namespace luvk_example;

constexpr auto g_VertexShader = R"(
    #version 450

    layout(location = 0) in vec2 inPos;
    layout(location = 1) in vec2 offset;
    layout(location = 2) in float angle;
    layout(location = 3) in vec4 color;

    layout(location = 0) out vec4 vColor;

    void main()
    {
       mat2 R = mat2(cos(angle), -sin(angle),
                     sin(angle),  cos(angle));
       vec2 pos = R * inPos + offset;
       gl_Position = vec4(pos, 0.0, 1.0);
       vColor = color;
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

constexpr auto g_ComputeShader = R"(
    #version 450

    layout(local_size_x = 64) in;

    struct Particle
    {
       vec2 offset;
       float angle;
       vec4 color;
       vec2 velocity;
    };

    layout(std430, binding = 0) buffer PartData
    {
       Particle particles[];
    };

    layout(push_constant) uniform PC
    {
       float dt;
    } pc;

    void main()
    {
       uint id = gl_GlobalInvocationID.x;
       particles[id].angle += pc.dt * 2.0;
    }
)";

constexpr std::array<glm::vec2, 3> g_Vertices{{{-0.05F, -0.05F},
                                               {0.05F, -0.05F},
                                               {0.F, 0.1F}}};

constexpr std::array<std::uint16_t, 3> g_Indices{{0, 1, 2}};

constexpr VkDescriptorSetLayoutBinding g_DescriptorBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
constexpr VkDescriptorPoolSize         g_DescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};

constexpr std::array g_Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                VkVertexInputBindingDescription{1, sizeof(Particle), VK_VERTEX_INPUT_RATE_INSTANCE}};

constexpr std::array g_Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                                  VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, Offset)},
                                  VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(Particle, Angle)},
                                  VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, Color)}};

constexpr VkPushConstantRange g_ConstantRange{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};

Triangle::Triangle(const std::shared_ptr<luvk::Device>&         Device,
                   const std::shared_ptr<luvk::SwapChain>&      Swap,
                   const std::shared_ptr<luvk::Memory>&         Memory,
                   const std::shared_ptr<luvk::DescriptorPool>& Pool)
    : Mesh(Device, Memory),
      m_ComputePipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_ParticleBuffer(std::make_shared<luvk::Buffer>(Device, Memory)),
      m_ComputeUBO(std::make_shared<luvk::Buffer>(Device, Memory)),
      m_GraphicsMat(std::make_shared<luvk::Material>()),
      m_ComputeMat(std::make_shared<luvk::Material>())
{
    const auto Set = std::make_shared<luvk::DescriptorSet>(Device, Pool, Memory);
    Set->CreateLayout({.Bindings = std::array{g_DescriptorBinding}});
    Set->Allocate();

    const auto GraphicsPipeline = std::make_shared<luvk::Pipeline>(Device);
    GraphicsPipeline->CreateGraphicsPipeline({.Extent = Swap->GetExtent(),
                                              .ColorFormats = std::array{Swap->GetCreationArguments().Format},
                                              .RenderPass = Swap->GetRenderPass(),
                                              .Subpass = 0,
                                              .VertexShader = luvk::CompileGLSLToSPIRV(g_VertexShader, EShLangVertex),
                                              .FragmentShader = luvk::CompileGLSLToSPIRV(g_FragmentShader, EShLangFragment),
                                              .SetLayouts = std::array{Set->GetLayout()},
                                              .Bindings = g_Bindings,
                                              .Attributes = g_Attributes,
                                              .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                              .CullMode = VK_CULL_MODE_NONE});

    m_ComputePipeline->CreateComputePipeline({.ComputeShader = luvk::CompileGLSLToSPIRV(g_ComputeShader, EShLangCompute),
                                              .SetLayouts = std::array{Set->GetLayout()},
                                              .PushConstants = std::array{g_ConstantRange}});

    m_ParticleBuffer->CreateBuffer({.Name = "Particle VTX",
                                    .Size = sizeof(Particle) * 1000,
                                    .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                    .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    Set->UpdateBuffer(m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    m_GraphicsMat->SetPipeline(GraphicsPipeline);
    m_GraphicsMat->SetDescriptorSet(Set);

    m_ComputeMat->SetPipeline(m_ComputePipeline);
    m_ComputeMat->SetDescriptorSet(Set);

    SetMaterial(m_GraphicsMat);
    UploadVertices(std::as_bytes(std::span{g_Vertices}), 3);
    UploadIndices(std::span{g_Indices});
}

void Triangle::Compute(const VkCommandBuffer& Cmd) const
{
    if (std::empty(m_Particles))
    {
        return;
    }

    static float DT = 0.016f;

    m_ComputeMat->Bind(Cmd);
    vkCmdPushConstants(Cmd, m_ComputePipeline->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &DT);
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

void Triangle::Draw(const VkCommandBuffer& Cmd, std::span<const std::byte> PushConstants) const
{
    m_GraphicsMat->Bind(Cmd);

    const std::array                      Buffers = {m_VertexBuffer->GetHandle(), m_ParticleBuffer->GetHandle()};
    constexpr std::array<VkDeviceSize, 2> Offsets = {0, 0};

    vkCmdBindVertexBuffers(Cmd, 0, static_cast<uint32_t>(std::size(Buffers)), std::data(Buffers), std::data(Offsets));

    vkCmdBindIndexBuffer(Cmd, m_IndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(Cmd, m_IndexCount, m_InstanceCount, 0, 0, 0);
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
