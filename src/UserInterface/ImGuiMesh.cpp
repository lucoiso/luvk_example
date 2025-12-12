// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiMesh.hpp"
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Types/Material.hpp>
#include <volk/volk.h>

using namespace luvk_example;

constexpr auto g_VertexShader = R"(
    #version 450

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
    }
)";

constexpr auto g_FragmentShader = R"(
    #version 450

    layout(set = 0, binding = 0) uniform sampler2D Font;

    layout(location = 0) in vec2 fragUV;
    layout(location = 1) in vec4 fragColor;
    layout(location = 0) out vec4 outColor;

    void main()
    {
        outColor = fragColor * texture(Font, fragUV);
    }
)";

constexpr std::uint32_t g_DescriptorCount = 100U;

constexpr std::array g_DescriptorPoolSizes{VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER,                g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, g_DescriptorCount},
                                           VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       g_DescriptorCount}};

constexpr VkDescriptorSetLayoutBinding g_DescriptorBinding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};

constexpr std::array g_Bindings{VkVertexInputBindingDescription{0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}};

constexpr std::array g_Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT,  offsetof(ImDrawVert, pos)},
                                  VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT,  offsetof(ImDrawVert, uv)},
                                  VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}};

ImGuiMesh::ImGuiMesh(const std::shared_ptr<luvk::MeshRegistry>& Registry,
                     const std::shared_ptr<luvk::Device>& Device,
                     const std::shared_ptr<luvk::SwapChain>& Swap,
                     const std::shared_ptr<luvk::Memory>& Memory)
    : m_DescPool(std::make_shared<luvk::DescriptorPool>(Device)),
      m_FontSet(std::make_shared<luvk::DescriptorSet>(Device, m_DescPool, Memory)),
      m_FontImage(std::make_shared<luvk::Image>(Device, Memory)),
      m_FontSampler(std::make_shared<luvk::Sampler>(Device)),
      m_Pipeline(std::make_shared<luvk::Pipeline>(Device)),
      m_Registry(Registry),
      m_Device(Device),
      m_SwapChain(Swap),
      m_Memory(Memory)
{
    m_DescPool->CreateDescriptorPool(std::size(g_DescriptorPoolSizes), g_DescriptorPoolSizes);

    m_FontSet->CreateLayout({.Bindings = std::array{g_DescriptorBinding}});
    m_FontSet->Allocate();

    ImGuiIO& IO = ImGui::GetIO();
    IO.BackendRendererName = "ImGuiBackendLUVK";
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    unsigned char* Pixels = nullptr;
    std::int32_t Width = 0;
    std::int32_t Height = 0;
    IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

    const auto Staging = std::make_shared<luvk::Buffer>(Device, Memory);
    Staging->CreateBuffer({.Name = "ImGUI Staging",
                           .Size = static_cast<VkDeviceSize>(Width * Height * 4),
                           .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    Staging->Upload({reinterpret_cast<std::byte const*>(Pixels), static_cast<size_t>(Width * Height * 4)});

    m_FontImage->CreateImage({.Extent = {static_cast<std::uint32_t>(Width), static_cast<std::uint32_t>(Height), 1},
                              .Format = VK_FORMAT_R8G8B8A8_UNORM,
                              .Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                              .Aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                              .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY});

    m_FontImage->Upload(Staging);

    m_FontSampler->CreateSampler({.Filter = VK_FILTER_LINEAR,
                                  .AddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE});

    m_FontSet->UpdateImage(m_FontImage->GetView(), m_FontSampler->GetHandle(), 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(m_FontSet->GetHandle()));

    constexpr VkPushConstantRange PCRange{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4};

    m_Pipeline->CreateGraphicsPipeline({.Extent = m_SwapChain->GetExtent(),
                                        .ColorFormats = std::array{m_SwapChain->GetCreationArguments().Format},
                                        .RenderPass = m_SwapChain->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = luvk::CompileGLSLToSPIRV(g_VertexShader, EShLangVertex),
                                        .FragmentShader = luvk::CompileGLSLToSPIRV(g_FragmentShader, EShLangFragment),
                                        .SetLayouts = std::array{m_FontSet->GetLayout()},
                                        .Bindings = g_Bindings,
                                        .Attributes = g_Attributes,
                                        .PushConstants = std::array{PCRange},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE,
                                        .EnableDepthOp = false});

    m_Index = m_Registry->RegisterMesh({},
                                       {},
                                       m_FontSet->GetLayout(),
                                       m_DescPool,
                                       m_FontImage,
                                       m_FontSampler,
                                       nullptr,
                                       {},
                                       m_Pipeline);

    auto& MeshEntry = const_cast<luvk::MeshEntry&>(m_Registry->GetMeshes()[m_Index]);
    MeshEntry.MaterialPtr->SetDescriptor(m_FontSet);
    MeshEntry.InstanceCount = 1;

    m_Mesh = std::make_shared<luvk::Mesh>(m_Registry, m_Index);
}

void ImGuiMesh::NewFrame() const
{
    ImGuiIO& GuiIO = ImGui::GetIO();
    GuiIO.DeltaTime = GuiIO.DeltaTime > 0 ? GuiIO.DeltaTime : 1.F / 60.F;
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

    const auto FbWidth = static_cast<std::int32_t>(RenderData->DisplaySize.x * RenderData->FramebufferScale.x);
    const auto FbHeight = static_cast<std::int32_t>(RenderData->DisplaySize.y * RenderData->FramebufferScale.y);
    if (FbWidth <= 0 || FbHeight <= 0)
    {
        return;
    }

    std::size_t const VertexSize = RenderData->TotalVtxCount * sizeof(ImDrawVert);
    std::size_t const IndexSize = RenderData->TotalIdxCount * sizeof(ImDrawIdx);

    if (!m_VtxBuffer || m_VtxBufferSize < VertexSize)
    {
        m_VtxBufferSize = VertexSize;
        if (!m_VtxBuffer)
        {
            m_VtxBuffer = std::make_shared<luvk::Buffer>(m_Device, m_Memory);
        }

        m_VtxBuffer->CreateBuffer({.Name = "ImGUI VTX",
                                   .Size = m_VtxBufferSize,
                                   .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                   .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }

    if (!m_IdxBuffer || m_IdxBufferSize < IndexSize)
    {
        m_IdxBufferSize = IndexSize;
        if (!m_IdxBuffer)
        {
            m_IdxBuffer = std::make_shared<luvk::Buffer>(m_Device, m_Memory);
        }

        m_IdxBuffer->CreateBuffer({.Name = "ImGUI IDX",
                                   .Size = m_IdxBufferSize,
                                   .Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                   .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }

    if (m_Vertices.capacity() < static_cast<std::size_t>(RenderData->TotalVtxCount))
    {
        m_Vertices.reserve(RenderData->TotalVtxCount);
    }
    if (m_Indices.capacity() < static_cast<std::size_t>(RenderData->TotalIdxCount))
    {
        m_Indices.reserve(RenderData->TotalIdxCount);
    }

    m_Vertices.resize(RenderData->TotalVtxCount);
    m_Indices.resize(RenderData->TotalIdxCount);

    std::int32_t VtxOffset = 0;
    std::int32_t IdxOffset = 0;
    for (std::int32_t ListIt = 0; ListIt < RenderData->CmdListsCount; ListIt++)
    {
        ImDrawList const* CmdList = RenderData->CmdLists[ListIt];

        std::memcpy(std::data(m_Vertices) + VtxOffset,
                    CmdList->VtxBuffer.Data,
                    CmdList->VtxBuffer.Size * sizeof(ImDrawVert));

        std::memcpy(std::data(m_Indices) + IdxOffset,
                    CmdList->IdxBuffer.Data,
                    CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

        VtxOffset += CmdList->VtxBuffer.Size;
        IdxOffset += CmdList->IdxBuffer.Size;
    }

    m_VtxBuffer->Upload(std::as_bytes(std::span{m_Vertices}));
    m_IdxBuffer->Upload(std::as_bytes(std::span{m_Indices}));

    const std::array Push{2.F / RenderData->DisplaySize.x,
                          2.F / RenderData->DisplaySize.y,
                          -1.F - RenderData->DisplayPos.x * (2.F / RenderData->DisplaySize.x),
                          -1.F - RenderData->DisplayPos.y * (2.F / RenderData->DisplaySize.y)};

    const VkBuffer& VtxHandle = m_VtxBuffer->GetHandle();
    constexpr VkDeviceSize VtxBufOffset = 0;

    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipeline());
    vkCmdBindVertexBuffers(Cmd, 0, 1, &VtxHandle, &VtxBufOffset);
    vkCmdBindIndexBuffer(Cmd,
                         m_IdxBuffer->GetHandle(),
                         0,
                         sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

    constexpr std::size_t NumPushConstant = std::size(Push) * sizeof(float);
    const auto PushConstantData = std::data(Push);

    vkCmdPushConstants(Cmd, m_Pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, NumPushConstant, PushConstantData);

    VkViewport const Viewport{.x = 0.F,
                              .y = 0.F,
                              .width = static_cast<float>(FbWidth),
                              .height = static_cast<float>(FbHeight),
                              .minDepth = 0.F,
                              .maxDepth = 1.F};
    vkCmdSetViewport(Cmd, 0, 1, &Viewport);

    ImVec2 const ClipOff = RenderData->DisplayPos;
    ImVec2 const ClipScale = RenderData->FramebufferScale;

    std::int32_t GlobalVtxOffset = 0;
    std::int32_t GlobalIdxOffset = 0;
    for (std::int32_t ListIt = 0; ListIt < RenderData->CmdListsCount; ListIt++)
    {
        ImDrawList const* CmdList = RenderData->CmdLists[ListIt];
        for (std::int32_t CommandIt = 0; CommandIt < CmdList->CmdBuffer.Size; CommandIt++)
        {
            ImDrawCmd const* Pcmd = &CmdList->CmdBuffer[CommandIt];
            if (Pcmd->UserCallback)
            {
                if (Pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                {
                    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipeline());
                    vkCmdBindVertexBuffers(Cmd, 0, 1, &VtxHandle, &VtxBufOffset);
                    vkCmdBindIndexBuffer(Cmd,
                                         m_IdxBuffer->GetHandle(),
                                         0,
                                         sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
                    vkCmdPushConstants(Cmd, m_Pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, NumPushConstant, PushConstantData);
                }
                else
                {
                    Pcmd->UserCallback(CmdList, Pcmd);
                }

                continue;
            }

            ImVec2 ClipMin((Pcmd->ClipRect.x - ClipOff.x) * ClipScale.x,
                           (Pcmd->ClipRect.y - ClipOff.y) * ClipScale.y);

            ImVec2 ClipMax((Pcmd->ClipRect.z - ClipOff.x) * ClipScale.x,
                           (Pcmd->ClipRect.w - ClipOff.y) * ClipScale.y);

            if (ClipMin.x < 0.F)
            {
                ClipMin.x = 0.F;
            }
            if (ClipMin.y < 0.F)
            {
                ClipMin.y = 0.F;
            }
            if (ClipMax.x > static_cast<float>(FbWidth))
            {
                ClipMax.x = static_cast<float>(FbWidth);
            }
            if (ClipMax.y > static_cast<float>(FbHeight))
            {
                ClipMax.y = static_cast<float>(FbHeight);
            }
            if (ClipMax.x <= ClipMin.x || ClipMax.y <= ClipMin.y)
            {
                continue;
            }

            VkRect2D const LocalScissor{.offset = {.x = static_cast<std::int32_t>(ClipMin.x),
                                                   .y = static_cast<std::int32_t>(ClipMin.y)},
                                        .extent = {.width = static_cast<std::uint32_t>(ClipMax.x - ClipMin.x),
                                                   .height = static_cast<std::uint32_t>(ClipMax.y - ClipMin.y)}};
            
            vkCmdSetScissor(Cmd, 0, 1, &LocalScissor);

            const auto DescSet = reinterpret_cast<VkDescriptorSet>(Pcmd->GetTexID());
            vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipelineLayout(), 0, 1, &DescSet, 0, nullptr);
            vkCmdDrawIndexed(Cmd, Pcmd->ElemCount, 1, Pcmd->IdxOffset + GlobalIdxOffset, Pcmd->VtxOffset + GlobalVtxOffset, 0);
        }

        GlobalIdxOffset += CmdList->IdxBuffer.Size;
        GlobalVtxOffset += CmdList->VtxBuffer.Size;
    }

    VkRect2D const FullScissor{{0, 0}, {static_cast<std::uint32_t>(FbWidth), static_cast<std::uint32_t>(FbHeight)}};
    vkCmdSetScissor(Cmd, 0, 1, &FullScissor);
}
