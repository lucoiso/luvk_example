// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Triangle.hpp"

#include <random>
#include <array>
#include <glm/gtc/constants.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

namespace
{
    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    { 0.05f, -0.05f},
                                                    { 0.0f,   0.1f}}};
    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};

    constexpr auto VertSrc = R"(#version 450
                                layout(location = 0) in vec2 inPos;
                                layout(location = 1) in vec2 offset;
                                layout(location = 2) in float angle;
                                layout(location = 3) in vec4 instColor;
                                layout(location = 0) out vec4 vColor;
                                void main() {
                                    mat2 R = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
                                    vec2 pos = R * inPos + offset;
                                    gl_Position = vec4(pos, 0.0, 1.0);
                                    vColor = instColor;
                                })";

    constexpr auto FragSrc = R"(#version 450
                                layout(location = 0) in vec4 vColor;
                                layout(location = 0) out vec4 outColor;
                                void main() {
                                    outColor = vColor;
                                })";

    constexpr auto CompSrc = R"(#version 450
                                layout(local_size_x = 1) in;
                                struct Instance {
                                    vec2 offset;
                                    float angle;
                                    vec4 color;
                                };
                                layout(std430, binding = 0) buffer Instances { Instance inst[]; };
                                layout(push_constant) uniform Push { float dt; } pc;
                                void main() {
                                    uint id = gl_GlobalInvocationID.x;
                                    inst[id].angle += pc.dt;
                                })";
}

Triangle::Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                   std::shared_ptr<luvk::Device> Device,
                   std::shared_ptr<luvk::SwapChain> SwapChain,
                   std::shared_ptr<luvk::CommandPool> CmdPool)
    : m_Registry(std::move(Registry)), m_Device(std::move(Device)), m_CommandPool(std::move(CmdPool))
{
    m_Pipeline = std::make_shared<luvk::Pipeline>();
    m_ComputePipeline = std::make_shared<luvk::Pipeline>();
    m_CompPool = std::make_shared<luvk::DescriptorPool>();
    m_CompDesc = std::make_shared<luvk::DescriptorSet>();

    auto Vert = luvk::CompileGLSLToSPIRV(VertSrc, EShLangVertex);
    auto Frag = luvk::CompileGLSLToSPIRV(FragSrc, EShLangFragment);
    auto Comp = luvk::CompileGLSLToSPIRV(CompSrc, EShLangCompute);

    const VkExtent2D Extent = SwapChain->GetExtent();
    const std::array Formats{SwapChain->m_Arguments.Format};
    constexpr std::array Bindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                  VkVertexInputBindingDescription{1, sizeof(luvk::MeshInstance), VK_VERTEX_INPUT_RATE_INSTANCE}};
    const std::array Attrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                           VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::MeshInstance, Offset)},
                           VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::MeshInstance, Angle)},
                           VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::MeshInstance, Color)}};

    m_Pipeline->CreateGraphicsPipeline(m_Device,
                                       {.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = SwapChain->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = Vert,
                                        .FragmentShader = Frag,
                                        .SetLayouts = {},
                                        .Bindings = std::array{Bindings.at(0), Bindings.at(1)},
                                        .Attributes = std::array{Attrs.at(0), Attrs.at(1), Attrs.at(2), Attrs.at(3)},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    const std::array PoolSz{VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}};
    m_CompPool->CreateDescriptorPool(m_Device, 1, PoolSz);
    const VkDescriptorSetLayoutBinding CompBinding{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr};
    m_CompDesc->CreateLayout(m_Device, {.Bindings = std::array{CompBinding}});
    m_CompDesc->Allocate(m_Device, m_CompPool);

    constexpr VkPushConstantRange CPC{VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float)};
    m_ComputePipeline->CreateComputePipeline(m_Device,
                                            {.ComputeShader = Comp,
                                             .SetLayouts = std::array{m_CompDesc->GetLayout()},
                                             .PushConstants = std::array{CPC}});

    m_Index = m_Registry->RegisterMesh(std::as_bytes(std::span{TriVertices}),
                                       std::as_bytes(std::span{TriIndices}),
                                       VK_NULL_HANDLE,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       nullptr,
                                       {},
                                       m_Pipeline,
                                       m_Device);
    m_Mesh = luvk::Mesh(m_Registry, m_Index);

    auto const& Entry = m_Registry->GetMeshes()[m_Index];
    VkDescriptorBufferInfo Buf{Entry.InstanceBuffer->GetHandle(), 0, Entry.InstanceBuffer->GetSize()};
    VkWriteDescriptorSet Write{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                               .pNext = nullptr,
                               .dstSet = m_CompDesc->GetHandle(),
                               .dstBinding = 0,
                               .descriptorCount = 1,
                               .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                               .pBufferInfo = &Buf};
    vkUpdateDescriptorSets(m_Device->GetLogicalDevice(), 1, &Write, 0, nullptr);
}

void Triangle::Update(float DeltaTime)
{
    auto Cmd = m_CommandPool->AllocateBuffers(m_Device->GetLogicalDevice(), 1).front();
    constexpr VkCommandBufferBeginInfo Begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(Cmd, &Begin);
    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->GetPipeline());
    vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline->GetPipelineLayout(), 0, 1, &m_CompDesc->GetHandle(), 0, nullptr);
    vkCmdPushConstants(Cmd, m_ComputePipeline->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(float), &DeltaTime);
    vkCmdDispatch(Cmd, m_Mesh.GetInstanceCount(), 1, 1);
    vkEndCommandBuffer(Cmd);
    VkSubmitInfo Submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    Submit.commandBufferCount = 1;
    Submit.pCommandBuffers = &Cmd;
    const auto Queue = m_Device->GetQueue(m_Device->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value());
    vkQueueSubmit(Queue, 1, &Submit, VK_NULL_HANDLE);
    vkQueueWaitIdle(Queue);
}

void Triangle::AddInstance(glm::vec2 Position)
{
    static std::mt19937 Generator{std::random_device{}()};
    static std::uniform_real_distribution Distribution(0.f, 1.f);

    luvk::MeshRegistry::InstanceInfo Instance{
        .XForm = {.Position = {Position.x, Position.y, 0.f}, .Rotation = std::array{0.f, 0.f, Distribution(Generator) * glm::two_pi<float>()}},
        .Color = {Distribution(Generator), Distribution(Generator), Distribution(Generator), 1.f}};
    m_Instances.push_back(std::move(Instance));
    if (m_Registry)
    {
        m_Registry->UpdateInstances(m_Index, m_Instances);
    }
}

luvk::Mesh& Triangle::GetMesh() noexcept
{
    return m_Mesh;
}
