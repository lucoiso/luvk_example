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

                                   struct Particle
                                   {
                                       vec2 offset;
                                       float angle;
                                       vec4 color;
                                       vec2 velocity;
                                   };

                                   layout(std430, binding = 0) readonly buffer PartData
                                   {
                                       Particle particles[];
                                   };

                                   layout(location = 0) out vec4 vColor;

                                   void main()
                                   {
                                       Particle p = particles[gl_InstanceIndex];
                                       mat2 R = mat2(cos(p.angle), -sin(p.angle), sin(p.angle), cos(p.angle));
                                       vec2 pos = R * inPos + p.offset;
                                       gl_Position = vec4(pos, 0.0, 1.0);
                                       vColor = p.color;
                                   })";

    constexpr auto TriFragSrc = R"(#version 450

                                   layout(location = 0) in vec4 vColor;
                                   layout(location = 0) out vec4 outColor;

                                   void main()
                                   {
                                       outColor = vColor;
                                   })";

    constexpr auto TriCompSrc = R"(#version 450

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
                                     Particle p = particles[id];
                                     p.velocity.y -= 9.81 * pc.dt;
                                     p.offset += p.velocity * pc.dt;
                                     particles[id] = p;
                                 })";

    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    {0.05f, -0.05f},
                                                    {0.0f, 0.1f}}};

    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};
} // namespace

luvk_example::Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                                 const std::shared_ptr<luvk::Device>& Device,
                                 const std::shared_ptr<luvk::SwapChain>& Swap,
                                 const std::shared_ptr<luvk::Memory>& Memory)
    : m_Registry(std::move(Registry)),
      m_GraphicsPipeline(std::make_shared<luvk::Pipeline>()),
      m_ComputePipeline(std::make_shared<luvk::Pipeline>()),
      m_ParticleBuffer(std::make_shared<luvk::Buffer>()),
      m_ComputeUBO(std::make_shared<luvk::Buffer>()),
      m_DescriptorPool(std::make_shared<luvk::DescriptorPool>()),
      m_DescriptorSet(std::make_shared<luvk::DescriptorSet>()),
      m_Device(Device)
{
    auto TriVert = luvk::CompileGLSLToSPIRV(TriVertSrc, EShLangVertex);
    auto TriFrag = luvk::CompileGLSLToSPIRV(TriFragSrc, EShLangFragment);
    auto TriComp = luvk::CompileGLSLToSPIRV(TriCompSrc, EShLangCompute);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->m_Arguments.Format};

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(Particle), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(Particle, Angle)},
                              VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, Color)}};

    VkDescriptorSetLayoutBinding PartBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
    m_DescriptorSet->CreateLayout(Device, {.Bindings = std::array{PartBinding}});
    VkDescriptorPoolSize PoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};
    m_DescriptorPool->CreateDescriptorPool(Device, 1, std::array{PoolSize});
    m_DescriptorSet->Allocate(Device, m_DescriptorPool);

    m_GraphicsPipeline->CreateGraphicsPipeline(Device,
                                               {.Extent = Extent,
                                                .ColorFormats = Formats,
                                                .RenderPass = Swap->GetRenderPass(),
                                                .Subpass = 0,
                                                .VertexShader = TriVert,
                                                .FragmentShader = TriFrag,
                                                .SetLayouts = std::array{m_DescriptorSet->GetLayout()},
                                                .Bindings = std::array{TriBindings.at(0), TriBindings.at(1)},
                                                .Attributes = std::array{TriAttrs.at(0), TriAttrs.at(1), TriAttrs.at(2), TriAttrs.at(3)},
                                                .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                                .CullMode = VK_CULL_MODE_NONE});

    constexpr VkPushConstantRange CompPC{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};
    m_ComputePipeline->CreateComputePipeline(Device,
                                             {.ComputeShader = TriComp,
                                              .SetLayouts = std::array{m_DescriptorSet->GetLayout()},
                                              .PushConstants = std::array{CompPC}});

    m_ParticleBuffer->CreateBuffer(Memory, {.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                            .Size = sizeof(Particle),
                                            .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_ComputeUBO->CreateBuffer(Memory,
                               {.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                .Size = sizeof(float),
                                .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_DescriptorSet->UpdateBuffer(m_Device, m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    m_GraphicsIndex = m_Registry->RegisterMesh(std::as_bytes(std::span{TriVertices}),
                                               std::as_bytes(std::span{TriIndices}),
                                               VK_NULL_HANDLE,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               nullptr,
                                               {},
                                               m_GraphicsPipeline,
                                               Device);

    m_ComputeIndex = m_Registry->RegisterMesh({},
                                              {},
                                              VK_NULL_HANDLE,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              m_ComputeUBO,
                                              {},
                                              m_ComputePipeline,
                                              Device,
                                              1);

    auto& gfxEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_GraphicsIndex]);
    gfxEntry.InstanceBuffer = m_ParticleBuffer;
    gfxEntry.MaterialPtr->SetDescriptor(m_DescriptorSet);

    auto& compEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_ComputeIndex]);
    compEntry.MaterialPtr->SetDescriptor(m_DescriptorSet);

    m_Mesh = luvk::Mesh(m_Registry, m_GraphicsIndex);
}

void luvk_example::Triangle::Update(const float DeltaTime)
{
    m_Registry->UpdateUniform(m_ComputeIndex, std::as_bytes(std::span{&DeltaTime, 1}));
}

void luvk_example::Triangle::AddInstance(const glm::vec2 Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    Particle P{};
    P.Offset = Position;
    P.Angle = Distribution(Generator) * glm::two_pi<float>();
    P.Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F};
    P.Velocity = {0.F, 0.F};

    m_Particles.push_back(P);

    VkDeviceSize Required = sizeof(Particle) * m_Particles.size();
    if (Required > m_ParticleBuffer->GetSize())
    {
        m_ParticleBuffer->RecreateBuffer({.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                          .Size = Required,
                                          .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
        m_DescriptorSet->UpdateBuffer(m_Device, m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        auto& gfxEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_GraphicsIndex]);
        gfxEntry.InstanceBuffer = m_ParticleBuffer;
    }

    m_ParticleBuffer->Upload(std::as_bytes(std::span{m_Particles}));

    auto& gfxEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_GraphicsIndex]);
    gfxEntry.InstanceCount = static_cast<std::uint32_t>(m_Particles.size());

    auto& compEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_ComputeIndex]);
    compEntry.DispatchX = static_cast<std::uint32_t>((m_Particles.size() + 63) / 64);
}

luvk::Mesh& luvk_example::Triangle::GetMesh() noexcept
{
    return m_Mesh;
}
