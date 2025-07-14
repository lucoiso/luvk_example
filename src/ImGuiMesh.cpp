#include "luvk_example/ImGuiMesh.hpp"
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <imgui.h>
#include <vector>
#include <cstring>

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
    constexpr VkDescriptorPoolSize PoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};
    m_DescPool->CreateDescriptorPool(m_Device, 1, std::array{PoolSize});

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

    auto vert = luvk::CompileGLSLToSPIRV(VertSrc, EShLangVertex);
    auto frag = luvk::CompileGLSLToSPIRV(FragSrc, EShLangFragment);

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
                                        .VertexShader = vert,
                                        .FragmentShader = frag,
                                        .SetLayouts = std::array{m_FontSet->GetLayout()},
                                        .Bindings = Bindings,
                                        .Attributes = Attrs,
                                        .PushConstants = std::array{PCRange},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    m_Index = m_Registry->RegisterMesh({}, {}, m_FontSet->GetLayout(), m_DescPool, m_FontImage, m_FontSampler, nullptr, {}, m_Pipeline, m_Device);
    auto& entry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_Index]);
    entry.MaterialPtr->SetDescriptor(m_FontSet);
    m_Mesh = luvk::Mesh(m_Registry, m_Index);
}

void ImGuiMesh::NewFrame() const
{
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = io.DeltaTime > 0 ? io.DeltaTime : 1.0f / 60.0f;
    ImGui::NewFrame();
}

void ImGuiMesh::Render(const VkCommandBuffer& Cmd)
{
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData || drawData->TotalVtxCount == 0)
    {
        return;
    }

    const size_t vertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
    const size_t indexSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

    auto& entry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_Index]);

    if (!m_VtxBuffer || m_VtxBufferSize < vertexSize)
    {
        m_VtxBufferSize = vertexSize;
        if (!m_VtxBuffer)
            m_VtxBuffer = std::make_shared<luvk::Buffer>();
        m_VtxBuffer->CreateBuffer(m_Memory, {.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, .Size = m_VtxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
        entry.VertexBuffer = m_VtxBuffer;
    }

    if (!m_IdxBuffer || m_IdxBufferSize < indexSize)
    {
        m_IdxBufferSize = indexSize;
        if (!m_IdxBuffer)
            m_IdxBuffer = std::make_shared<luvk::Buffer>();
        m_IdxBuffer->CreateBuffer(m_Memory, {.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT, .Size = m_IdxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
        entry.IndexBuffer = m_IdxBuffer;
    }

    std::vector<ImDrawVert> vertices(drawData->TotalVtxCount);
    std::vector<ImDrawIdx> indices(drawData->TotalIdxCount);
    int vtxOffset = 0;
    int idxOffset = 0;
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmd = drawData->CmdLists[n];
        std::memcpy(vertices.data() + vtxOffset, cmd->VtxBuffer.Data, cmd->VtxBuffer.Size * sizeof(ImDrawVert));
        std::memcpy(indices.data() + idxOffset, cmd->IdxBuffer.Data, cmd->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxOffset += cmd->VtxBuffer.Size;
        idxOffset += cmd->IdxBuffer.Size;
    }

    m_VtxBuffer->Upload(std::as_bytes(std::span{vertices}));
    m_IdxBuffer->Upload(std::as_bytes(std::span{indices}));

    entry.IndexCount = static_cast<uint32_t>(drawData->TotalIdxCount);
    entry.UniformCache.resize(sizeof(float) * 4);
    float scale[2];
    float translate[2];
    scale[0] = 2.0f / drawData->DisplaySize.x;
    scale[1] = 2.0f / drawData->DisplaySize.y;
    translate[0] = -1.0f - drawData->DisplayPos.x * scale[0];
    translate[1] = -1.0f - drawData->DisplayPos.y * scale[1];
    std::memcpy(entry.UniformCache.data(), scale, sizeof(float) * 2);
    std::memcpy(static_cast<char*>(reinterpret_cast<void*>(entry.UniformCache.data())) + sizeof(float) * 2, translate, sizeof(float) * 2);

    m_Mesh.Draw(Cmd);
}

luvk::Mesh& ImGuiMesh::GetMesh() noexcept
{
    return m_Mesh;
}

