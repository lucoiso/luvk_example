// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/ImGuiMesh.hpp"
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Types/Material.hpp>
#include <imgui.h>
#include <vector>

using namespace luvk_example;

namespace
{
    constexpr auto VertSrc = R"(#version 450

                                layout(location = 0) in vec2 inPos;
                                layout(location = 1) in vec2 inUV;
                                layout(location = 2) in vec4 inColor;

                                layout(push_constant) uniform PC
                                {
                                    vec2 scale;
                                    vec2 translate;
                                } pc;

                                layout(location = 0) out vec2 fragUV;
                                layout(location = 1) out vec4 fragColor;

                                void main()
                                {
                                    fragUV = inUV;
                                    fragColor = inColor;
                                    gl_Position = vec4(inPos * pc.scale + pc.translate, 0.0, 1.0);
                                })";

    constexpr auto FragSrc = R"(#version 450

                                layout(set = 0, binding = 0) uniform sampler2D Font;

                                layout(location = 0) in vec2 fragUV;
                                layout(location = 1) in vec4 fragColor;
                                layout(location = 0) out vec4 outColor;

                                void main()
                                {
                                    outColor = fragColor * texture(Font, fragUV);
                                })";
}

ImGuiMesh::ImGuiMesh(std::shared_ptr<luvk::MeshRegistry> Registry,
                     std::shared_ptr<luvk::Device> Device,
                     std::shared_ptr<luvk::SwapChain> Swap,
                     std::shared_ptr<luvk::Memory> Memory)
    : m_Registry(std::move(Registry)),
      m_DescPool(std::make_shared<luvk::DescriptorPool>()),
      m_FontSet(std::make_shared<luvk::DescriptorSet>()),
      m_FontImage(std::make_shared<luvk::Image>()),
      m_FontSampler(std::make_shared<luvk::Sampler>()),
      m_Pipeline(std::make_shared<luvk::Pipeline>()),
      m_Device(std::move(Device)),
      m_SwapChain(std::move(Swap)),
      m_Memory(std::move(Memory))
{
    constexpr std::uint32_t DescriptorCount = 100U;

    constexpr std::array DescriptorPoolSizes{VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, DescriptorCount},
                                             VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, DescriptorCount}};

    m_DescPool->CreateDescriptorPool(m_Device, std::size(DescriptorPoolSizes), DescriptorPoolSizes);

    constexpr VkDescriptorSetLayoutBinding Binding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};
    m_FontSet->CreateLayout(m_Device, {.Bindings = std::array{Binding}});
    m_FontSet->Allocate(m_Device, m_DescPool, m_Memory);

    ImGuiIO& IO = ImGui::GetIO();
    IO.BackendRendererName = "ImGuiMesh";
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    unsigned char* Pixels;
    int Width, Height;
    IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

    m_FontImage->CreateImage(m_Device,
                             m_Memory,
                             {.Extent = {static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1},
                              .Format = VK_FORMAT_R8G8B8A8_UNORM,
                              .Usage = VK_IMAGE_USAGE_SAMPLED_BIT,
                              .Aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                              .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    m_FontImage->Upload({reinterpret_cast<std::byte const*>(Pixels), static_cast<size_t>(Width * Height * 4)});

    m_FontSampler->CreateSampler(m_Device, {});
    m_FontSet->UpdateImage(m_Device, m_FontImage->GetView(), m_FontSampler->GetHandle(), 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(m_FontSet->GetHandle()));

    auto VertexShader = luvk::CompileGLSLToSPIRV(VertSrc, EShLangVertex);
    auto FragmentShader = luvk::CompileGLSLToSPIRV(FragSrc, EShLangFragment);

    const VkExtent2D Extent = m_SwapChain->GetExtent();
    const std::array Formats{m_SwapChain->m_Arguments.Format};

    constexpr std::array Bindings{VkVertexInputBindingDescription{0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}};
    constexpr std::array Attrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)},
                               VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)},
                               VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}};

    constexpr VkPushConstantRange PCRange{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4};

    m_Pipeline->CreateGraphicsPipeline(m_Device,
                                       {.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = m_SwapChain->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = VertexShader,
                                        .FragmentShader = FragmentShader,
                                        .SetLayouts = std::array{m_FontSet->GetLayout()},
                                        .Bindings = Bindings,
                                        .Attributes = Attrs,
                                        .PushConstants = std::array{PCRange},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    m_Index = m_Registry->RegisterMesh({}, {}, m_FontSet->GetLayout(), m_DescPool, m_FontImage, m_FontSampler, nullptr, {}, m_Pipeline, m_Device);
    auto& MeshEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_Index]);
    MeshEntry.MaterialPtr->SetDescriptor(m_FontSet);
    MeshEntry.InstanceCount = 1;
    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void ImGuiMesh::NewFrame() const
{
    ImGuiIO& GuiIO = ImGui::GetIO();
    GuiIO.DeltaTime = GuiIO.DeltaTime > 0
                          ? GuiIO.DeltaTime
                          : 1.0f / 60.0f;
    ImGui::NewFrame();
}

void ImGuiMesh::Render(const VkCommandBuffer& Cmd)
{
    ImGui::Render();
    ImDrawData* RenderData = ImGui::GetDrawData();
    if (!RenderData || RenderData->TotalVtxCount == 0)
    {
        return;
    }

    const size_t VertexSize = RenderData->TotalVtxCount * sizeof(ImDrawVert);
    const size_t IndexSize = RenderData->TotalIdxCount * sizeof(ImDrawIdx);

    auto& MeshEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_Index]);

    if (!m_VtxBuffer || m_VtxBufferSize < VertexSize)
    {
        m_VtxBufferSize = VertexSize;
        if (!m_VtxBuffer)
        {
            m_VtxBuffer = std::make_shared<luvk::Buffer>();
        }

        m_VtxBuffer->CreateBuffer(m_Memory, {.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, .Size = m_VtxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
        MeshEntry.VertexBuffer = m_VtxBuffer;
    }

    if (!m_IdxBuffer || m_IdxBufferSize < IndexSize)
    {
        m_IdxBufferSize = IndexSize;
        if (!m_IdxBuffer)
        {
            m_IdxBuffer = std::make_shared<luvk::Buffer>();
        }

        m_IdxBuffer->CreateBuffer(m_Memory, {.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT, .Size = m_IdxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
        MeshEntry.IndexBuffer = m_IdxBuffer;
    }

    std::vector<ImDrawVert> Vertices(RenderData->TotalVtxCount);
    std::vector<ImDrawIdx> Indices(RenderData->TotalIdxCount);

    int vtxOffset = 0;
    int idxOffset = 0;
    for (int CmdListIt = 0; CmdListIt < RenderData->CmdListsCount; CmdListIt++)
    {
        const ImDrawList* DrawItem = RenderData->CmdLists[CmdListIt];
        std::memcpy(std::data(Vertices) + vtxOffset, DrawItem->VtxBuffer.Data, DrawItem->VtxBuffer.Size * sizeof(ImDrawVert));
        std::memcpy(std::data(Indices) + idxOffset, DrawItem->IdxBuffer.Data, DrawItem->IdxBuffer.Size * sizeof(ImDrawIdx));

        vtxOffset += DrawItem->VtxBuffer.Size;
        idxOffset += DrawItem->IdxBuffer.Size;
    }

    m_VtxBuffer->Upload(std::as_bytes(std::span{Vertices}));
    m_IdxBuffer->Upload(std::as_bytes(std::span{Indices}));

    MeshEntry.IndexCount = static_cast<uint32_t>(RenderData->TotalIdxCount);
    MeshEntry.UniformCache.resize(sizeof(float) * 4);

    float Scale[2];
    Scale[0] = 2.0f / RenderData->DisplaySize.x;
    Scale[1] = 2.0f / RenderData->DisplaySize.y;

    float Translate[2];
    Translate[0] = -1.0f - RenderData->DisplayPos.x * Scale[0];
    Translate[1] = -1.0f - RenderData->DisplayPos.y * Scale[1];

    std::memcpy(std::data(MeshEntry.UniformCache), Scale, sizeof(float) * 2);
    std::memcpy(static_cast<char*>(reinterpret_cast<void*>(std::data(MeshEntry.UniformCache))) + sizeof(float) * 2, Translate, sizeof(float) * 2);

    m_Mesh.Draw(Cmd);
}

luvk::Mesh& ImGuiMesh::GetMesh() noexcept
{
    return m_Mesh;
}
