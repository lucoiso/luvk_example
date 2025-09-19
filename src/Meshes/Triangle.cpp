// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Meshes/Triangle.hpp"
#include <array>
#include <random>
#include <glm/gtc/constants.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Types/Material.hpp>

using namespace luvk_example;

namespace
{
    constexpr auto TriVertSrc = R"(#version 450

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
                                       particles[id].angle += pc.dt * 2.0;
                                   })";

    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    {0.05f, -0.05f},
                                                    {0.F, 0.1f}}};

    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};
} // namespace

Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
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
    const std::array Formats{Swap->GetCreationArguments().Format};

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(Particle), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(Particle, Angle)},
                              VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, Color)}};

    constexpr VkDescriptorSetLayoutBinding PartBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
    constexpr VkDescriptorPoolSize PoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};

    m_DescriptorSet->CreateLayout(Device, {.Bindings = std::array{PartBinding}});
    m_DescriptorPool->CreateDescriptorPool(Device, 1, std::array{PoolSize});
    m_DescriptorSet->Allocate(m_DescriptorPool);

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

    m_ParticleBuffer->CreateBuffer(Memory,
                                   {.Size = sizeof(Particle),
                                    .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                    .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_ComputeUBO->CreateBuffer(Memory,
                               {.Size = sizeof(float),
                                .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_DescriptorSet->UpdateBuffer(m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

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

    const auto& compEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_ComputeIndex]);
    compEntry.MaterialPtr->SetDescriptor(m_DescriptorSet);

    m_Mesh = luvk::Mesh(m_Registry, m_GraphicsIndex);
}

void Triangle::Update(const float DeltaTime) const
{
    m_Registry->UpdateUniform(m_ComputeIndex, std::as_bytes(std::span{&DeltaTime, 1}));
}

void Triangle::AddInstance(glm::vec2 const& Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    Particle NewParticle{};
    NewParticle.Offset = Position;
    NewParticle.Angle = Distribution(Generator) * glm::two_pi<float>();
    NewParticle.Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F};
    NewParticle.Velocity = {0.F, 0.F};

    m_Particles.push_back(NewParticle);

    auto& GraphicsEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_GraphicsIndex]);
    auto& ComputeEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_ComputeIndex]);

    const std::size_t ParticlesSize = std::size(m_Particles);

    if (const VkDeviceSize Required = sizeof(Particle) * ParticlesSize;
        Required > m_ParticleBuffer->GetSize())
    {
        m_ParticleBuffer->RecreateBuffer({.Size = Required,
                                          .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                          .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

        m_DescriptorSet->UpdateBuffer(m_ParticleBuffer->GetHandle(), m_ParticleBuffer->GetSize(), 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

        GraphicsEntry.InstanceBuffer = m_ParticleBuffer;
    }

    m_ParticleBuffer->Upload(std::as_bytes(std::span{m_Particles}));

    GraphicsEntry.InstanceCount = static_cast<std::uint32_t>(ParticlesSize);
    ComputeEntry.DispatchX = static_cast<std::uint32_t>((ParticlesSize + 63) / 64);
}
