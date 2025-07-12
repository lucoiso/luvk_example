// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Triangle.hpp"
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/CommandPool.hpp>
#include <random>
#include <glm/gtc/constants.hpp>
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr auto TriVertSrc = R"(#version 450

                                   layout(location = 0) in vec2 inPos;
                                   layout(location = 1) in vec2 offset;
                                   layout(location = 2) in vec4 instColor;
                                   layout(location = 0) out vec4 vColor;

                                   void main()
                                   {
                                       vec2 pos = inPos + offset;
                                       gl_Position = vec4(pos, 0.0, 1.0);
                                       vColor = instColor;
                                   })";

    constexpr auto TriFragSrc = R"(#version 450

                                   layout(location = 0) in vec4 vColor;
                                   layout(location = 0) out vec4 outColor;

                                   void main()
                                   {
                                       outColor = vColor;
                                   })";

    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    {0.05f, -0.05f},
                                                    {0.0f, 0.1f}}};

    constexpr auto TriCompSrc = R"(#version 450
                                   layout(local_size_x = 1) in;

                                   struct Instance { vec2 pos; float vel; vec4 color; };
                                   layout(binding = 0) buffer Instances { Instance data[]; } buf;

                                   layout(push_constant) uniform Push { float dt; } pc;

                                   void main()
                                   {
                                       uint id = gl_GlobalInvocationID.x;
                                       buf.data[id].vel -= 9.8 * pc.dt;
                                       buf.data[id].pos.y += buf.data[id].vel * pc.dt;
                                   })";

    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};
} // namespace

luvk_example::Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                                 const std::shared_ptr<luvk::Device>& Device,
                                 const std::shared_ptr<luvk::SwapChain>& Swap,
                                 const std::shared_ptr<luvk::CommandPool>& CmdPool)
    : m_Registry(std::move(Registry)),
      m_Pipeline(std::make_shared<luvk::Pipeline>()),
      m_Compute(std::make_shared<luvk::Pipeline>()),
      m_Descriptor(std::make_shared<luvk::DescriptorSet>()),
      m_DescriptorPool(std::make_shared<luvk::DescriptorPool>())
{
    auto TriVert = luvk::CompileGLSLToSPIRV(TriVertSrc, EShLangVertex);
    auto TriFrag = luvk::CompileGLSLToSPIRV(TriFragSrc, EShLangFragment);

    const VkExtent2D Extent = Swap->GetExtent();
    const std::array Formats{Swap->m_Arguments.Format};

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(luvk::MeshInstance), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::MeshInstance, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::MeshInstance, Color)}};

    m_Pipeline->CreateGraphicsPipeline(Device,
                                       {.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = Swap->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = TriVert,
                                        .FragmentShader = TriFrag,
                                        .SetLayouts = {},
                                        .Bindings = std::array{TriBindings.at(0), TriBindings.at(1)},
                                        .Attributes = std::array{TriAttrs.at(0), TriAttrs.at(1), TriAttrs.at(2)},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{TriVertices}),
                                       std::as_bytes(std::span{TriIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       {},
                                       m_Pipeline,
                                       Device);

    m_Mesh = luvk::Mesh(m_Registry, m_Index);

    VkDescriptorSetLayoutBinding Bind{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
    m_Descriptor->CreateLayout(Device, {.Bindings = std::array{Bind}});

    constexpr VkDescriptorPoolSize PoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};
    m_DescriptorPool->CreateDescriptorPool(Device, 1, std::array{PoolSize});
    m_Descriptor->Allocate(Device, m_DescriptorPool);

    auto TriComp = luvk::CompileGLSLToSPIRV(TriCompSrc, EShLangCompute);
    constexpr VkPushConstantRange Pc{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};
    m_Compute->CreateComputePipeline(Device, {.ComputeShader = TriComp, .SetLayouts = std::array{m_Descriptor->GetLayout()}, .PushConstants = std::array{Pc}});

    const auto Buffers = CmdPool->AllocateBuffers(Device->GetLogicalDevice(), 1);
    m_ComputeCmd = Buffers.front();

    const auto qFamily = Device->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value();
    m_Queue = Device->GetQueue(qFamily);
}

void luvk_example::Triangle::AddInstance(const glm::vec2 Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.F, 1.F);

    luvk::MeshRegistry::InstanceInfo Instance{
        .XForm = {.Position = {Position.x, Position.y, 0.F}, .Rotation = std::array{0.F, 0.F, Distribution(Generator) * glm::two_pi<float>()}},
        .Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.F}};

    m_Instances.push_back(std::move(Instance));

    if (m_Registry)
    {
        m_Registry->UpdateInstances(m_Index, m_Instances);
    }
}

void luvk_example::Triangle::Update(const float dt)
{
    if (m_Instances.empty())
    {
        return;
    }

    vkResetCommandBuffer(m_ComputeCmd, 0);
    VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(m_ComputeCmd, &begin);
    vkCmdBindPipeline(m_ComputeCmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Compute->GetPipeline());
    vkCmdBindDescriptorSets(m_ComputeCmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_Compute->GetPipelineLayout(), 0, 1, &m_Descriptor->GetHandle(), 0, nullptr);
    vkCmdPushConstants(m_ComputeCmd, m_Compute->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &dt);
    vkCmdDispatch(m_ComputeCmd, static_cast<std::uint32_t>(m_Instances.size()), 1, 1);
    vkEndCommandBuffer(m_ComputeCmd);

    VkSubmitInfo submit{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO, .commandBufferCount = 1, .pCommandBuffers = &m_ComputeCmd};
    vkQueueSubmit(m_Queue, 1, &submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(m_Queue);
}

luvk::Mesh& luvk_example::Triangle::GetMesh() noexcept
{
    return m_Mesh;
}
