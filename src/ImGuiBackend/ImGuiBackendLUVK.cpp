// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/ImGuiBackend/ImGuiBackendLUVK.hpp"

#include "imgui.h"
#include <array>
#include <vector>
#include <span>

#include <glm/fwd.hpp>

static constexpr uint32_t ImGuiVertShaderSpv[] = {
    0x07230203,
    0x00010000,
    0x00080001,
    0x0000002e,
    0x00000000,
    0x00020011,
    0x00000001,
    0x0006000b,
    0x00000001,
    0x4c534c47,
    0x6474732e,
    0x3035342e,
    0x00000000,
    0x0003000e,
    0x00000000,
    0x00000001,
    0x000a000f,
    0x00000000,
    0x00000004,
    0x6e69616d,
    0x00000000,
    0x0000000b,
    0x0000000f,
    0x00000015,
    0x0000001b,
    0x0000001c,
    0x00030003,
    0x00000002,
    0x000001c2,
    0x00040005,
    0x00000004,
    0x6e69616d,
    0x00000000,
    0x00030005,
    0x00000009,
    0x00000000,
    0x00050006,
    0x00000009,
    0x00000000,
    0x6f6c6f43,
    0x00000072,
    0x00040006,
    0x00000009,
    0x00000001,
    0x00005655,
    0x00030005,
    0x0000000b,
    0x0074754f,
    0x00040005,
    0x0000000f,
    0x6c6f4361,
    0x0000726f,
    0x00030005,
    0x00000015,
    0x00565561,
    0x00060005,
    0x00000019,
    0x505f6c67,
    0x65567265,
    0x78657472,
    0x00000000,
    0x00060006,
    0x00000019,
    0x00000000,
    0x505f6c67,
    0x7469736f,
    0x006e6f69,
    0x00030005,
    0x0000001b,
    0x00000000,
    0x00040005,
    0x0000001c,
    0x736f5061,
    0x00000000,
    0x00060005,
    0x0000001e,
    0x73755075,
    0x6e6f4368,
    0x6e617473,
    0x00000074,
    0x00050006,
    0x0000001e,
    0x00000000,
    0x61635375,
    0x0000656c,
    0x00060006,
    0x0000001e,
    0x00000001,
    0x61725475,
    0x616c736e,
    0x00006574,
    0x00030005,
    0x00000020,
    0x00006370,
    0x00040047,
    0x0000000b,
    0x0000001e,
    0x00000000,
    0x00040047,
    0x0000000f,
    0x0000001e,
    0x00000002,
    0x00040047,
    0x00000015,
    0x0000001e,
    0x00000001,
    0x00050048,
    0x00000019,
    0x00000000,
    0x0000000b,
    0x00000000,
    0x00030047,
    0x00000019,
    0x00000002,
    0x00040047,
    0x0000001c,
    0x0000001e,
    0x00000000,
    0x00050048,
    0x0000001e,
    0x00000000,
    0x00000023,
    0x00000000,
    0x00050048,
    0x0000001e,
    0x00000001,
    0x00000023,
    0x00000008,
    0x00030047,
    0x0000001e,
    0x00000002,
    0x00020013,
    0x00000002,
    0x00030021,
    0x00000003,
    0x00000002,
    0x00030016,
    0x00000006,
    0x00000020,
    0x00040017,
    0x00000007,
    0x00000006,
    0x00000004,
    0x00040017,
    0x00000008,
    0x00000006,
    0x00000002,
    0x0004001e,
    0x00000009,
    0x00000007,
    0x00000008,
    0x00040020,
    0x0000000a,
    0x00000003,
    0x00000009,
    0x0004003b,
    0x0000000a,
    0x0000000b,
    0x00000003,
    0x00040015,
    0x0000000c,
    0x00000020,
    0x00000001,
    0x0004002b,
    0x0000000c,
    0x0000000d,
    0x00000000,
    0x00040020,
    0x0000000e,
    0x00000001,
    0x00000007,
    0x0004003b,
    0x0000000e,
    0x0000000f,
    0x00000001,
    0x00040020,
    0x00000011,
    0x00000003,
    0x00000007,
    0x0004002b,
    0x0000000c,
    0x00000013,
    0x00000001,
    0x00040020,
    0x00000014,
    0x00000001,
    0x00000008,
    0x0004003b,
    0x00000014,
    0x00000015,
    0x00000001,
    0x00040020,
    0x00000017,
    0x00000003,
    0x00000008,
    0x0003001e,
    0x00000019,
    0x00000007,
    0x00040020,
    0x0000001a,
    0x00000003,
    0x00000019,
    0x0004003b,
    0x0000001a,
    0x0000001b,
    0x00000003,
    0x0004003b,
    0x00000014,
    0x0000001c,
    0x00000001,
    0x0004001e,
    0x0000001e,
    0x00000008,
    0x00000008,
    0x00040020,
    0x0000001f,
    0x00000009,
    0x0000001e,
    0x0004003b,
    0x0000001f,
    0x00000020,
    0x00000009,
    0x00040020,
    0x00000021,
    0x00000009,
    0x00000008,
    0x0004002b,
    0x00000006,
    0x00000028,
    0x00000000,
    0x0004002b,
    0x00000006,
    0x00000029,
    0x3f800000,
    0x00050036,
    0x00000002,
    0x00000004,
    0x00000000,
    0x00000003,
    0x000200f8,
    0x00000005,
    0x0004003d,
    0x00000007,
    0x00000010,
    0x0000000f,
    0x00050041,
    0x00000011,
    0x00000012,
    0x0000000b,
    0x0000000d,
    0x0003003e,
    0x00000012,
    0x00000010,
    0x0004003d,
    0x00000008,
    0x00000016,
    0x00000015,
    0x00050041,
    0x00000017,
    0x00000018,
    0x0000000b,
    0x00000013,
    0x0003003e,
    0x00000018,
    0x00000016,
    0x0004003d,
    0x00000008,
    0x0000001d,
    0x0000001c,
    0x00050041,
    0x00000021,
    0x00000022,
    0x00000020,
    0x0000000d,
    0x0004003d,
    0x00000008,
    0x00000023,
    0x00000022,
    0x00050085,
    0x00000008,
    0x00000024,
    0x0000001d,
    0x00000023,
    0x00050041,
    0x00000021,
    0x00000025,
    0x00000020,
    0x00000013,
    0x0004003d,
    0x00000008,
    0x00000026,
    0x00000025,
    0x00050081,
    0x00000008,
    0x00000027,
    0x00000024,
    0x00000026,
    0x00050051,
    0x00000006,
    0x0000002a,
    0x00000027,
    0x00000000,
    0x00050051,
    0x00000006,
    0x0000002b,
    0x00000027,
    0x00000001,
    0x00070050,
    0x00000007,
    0x0000002c,
    0x0000002a,
    0x0000002b,
    0x00000028,
    0x00000029,
    0x00050041,
    0x00000011,
    0x0000002d,
    0x0000001b,
    0x0000000d,
    0x0003003e,
    0x0000002d,
    0x0000002c,
    0x000100fd,
    0x00010038
};

static constexpr uint32_t ImGuiFragShaderSpv[] = {
    0x07230203,
    0x00010000,
    0x00080001,
    0x0000001e,
    0x00000000,
    0x00020011,
    0x00000001,
    0x0006000b,
    0x00000001,
    0x4c534c47,
    0x6474732e,
    0x3035342e,
    0x00000000,
    0x0003000e,
    0x00000000,
    0x00000001,
    0x0007000f,
    0x00000004,
    0x00000004,
    0x6e69616d,
    0x00000000,
    0x00000009,
    0x0000000d,
    0x00030010,
    0x00000004,
    0x00000007,
    0x00030003,
    0x00000002,
    0x000001c2,
    0x00040005,
    0x00000004,
    0x6e69616d,
    0x00000000,
    0x00040005,
    0x00000009,
    0x6c6f4366,
    0x0000726f,
    0x00030005,
    0x0000000b,
    0x00000000,
    0x00050006,
    0x0000000b,
    0x00000000,
    0x6f6c6f43,
    0x00000072,
    0x00040006,
    0x0000000b,
    0x00000001,
    0x00005655,
    0x00030005,
    0x0000000d,
    0x00006e49,
    0x00050005,
    0x00000016,
    0x78655473,
    0x65727574,
    0x00000000,
    0x00040047,
    0x00000009,
    0x0000001e,
    0x00000000,
    0x00040047,
    0x0000000d,
    0x0000001e,
    0x00000000,
    0x00040047,
    0x00000016,
    0x00000022,
    0x00000000,
    0x00040047,
    0x00000016,
    0x00000021,
    0x00000000,
    0x00020013,
    0x00000002,
    0x00030021,
    0x00000003,
    0x00000002,
    0x00030016,
    0x00000006,
    0x00000020,
    0x00040017,
    0x00000007,
    0x00000006,
    0x00000004,
    0x00040020,
    0x00000008,
    0x00000003,
    0x00000007,
    0x0004003b,
    0x00000008,
    0x00000009,
    0x00000003,
    0x00040017,
    0x0000000a,
    0x00000006,
    0x00000002,
    0x0004001e,
    0x0000000b,
    0x00000007,
    0x0000000a,
    0x00040020,
    0x0000000c,
    0x00000001,
    0x0000000b,
    0x0004003b,
    0x0000000c,
    0x0000000d,
    0x00000001,
    0x00040015,
    0x0000000e,
    0x00000020,
    0x00000001,
    0x0004002b,
    0x0000000e,
    0x0000000f,
    0x00000000,
    0x00040020,
    0x00000010,
    0x00000001,
    0x00000007,
    0x00090019,
    0x00000013,
    0x00000006,
    0x00000001,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000001,
    0x00000000,
    0x0003001b,
    0x00000014,
    0x00000013,
    0x00040020,
    0x00000015,
    0x00000000,
    0x00000014,
    0x0004003b,
    0x00000015,
    0x00000016,
    0x00000000,
    0x0004002b,
    0x0000000e,
    0x00000018,
    0x00000001,
    0x00040020,
    0x00000019,
    0x00000001,
    0x0000000a,
    0x00050036,
    0x00000002,
    0x00000004,
    0x00000000,
    0x00000003,
    0x000200f8,
    0x00000005,
    0x00050041,
    0x00000010,
    0x00000011,
    0x0000000d,
    0x0000000f,
    0x0004003d,
    0x00000007,
    0x00000012,
    0x00000011,
    0x0004003d,
    0x00000014,
    0x00000017,
    0x00000016,
    0x00050041,
    0x00000019,
    0x0000001a,
    0x0000000d,
    0x00000018,
    0x0004003d,
    0x0000000a,
    0x0000001b,
    0x0000001a,
    0x00050057,
    0x00000007,
    0x0000001c,
    0x00000017,
    0x0000001b,
    0x00050085,
    0x00000007,
    0x0000001d,
    0x00000012,
    0x0000001c,
    0x0003003e,
    0x00000009,
    0x0000001d,
    0x000100fd,
    0x00010038
};

using namespace luvk_example;

bool ImGuiBackendLUVK::Init(std::shared_ptr<luvk::Renderer> const& Renderer)
{
    const auto DeviceModule = Renderer->FindModule<luvk::Device>();
    const auto MemoryModule = Renderer->FindModule<luvk::Memory>();
    const auto SwapChainModule = Renderer->FindModule<luvk::SwapChain>();

    m_DescPool = std::make_shared<luvk::DescriptorPool>();
    constexpr VkDescriptorPoolSize PoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1};
    m_DescPool->CreateDescriptorPool(DeviceModule, 1, std::array{PoolSize});

    m_FontSet = std::make_shared<luvk::DescriptorSet>();
    constexpr VkDescriptorSetLayoutBinding Binding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};
    m_FontSet->CreateLayout(DeviceModule, {.Bindings = std::array{Binding}});
    m_FontSet->Allocate(DeviceModule, m_DescPool, MemoryModule);

    ImGuiIO& IO = ImGui::GetIO();
    IO.BackendRendererName = "ImGuiBackendLUVK";
    IO.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    unsigned char* Pixels;
    int Width, Height;
    IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

    m_FontImage = std::make_shared<luvk::Image>();
    m_FontImage->CreateImage(DeviceModule,
                             MemoryModule,
                             {.Extent = {static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1},
                              .Format = VK_FORMAT_R8G8B8A8_UNORM,
                              .Usage = VK_IMAGE_USAGE_SAMPLED_BIT,
                              .Aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                              .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    m_FontImage->Upload({reinterpret_cast<std::byte const*>(Pixels), static_cast<size_t>(Width * Height * 4)});

    m_FontSampler = std::make_shared<luvk::Sampler>();
    m_FontSampler->CreateSampler(DeviceModule, {});

    m_FontSet->UpdateImage(DeviceModule, m_FontImage->GetView(), m_FontSampler->GetHandle(), 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(m_FontSet->GetHandle()));

    m_Pipeline = std::make_shared<luvk::Pipeline>();
    const VkExtent2D Extent = SwapChainModule->GetExtent();
    const std::array Formats{SwapChainModule->m_Arguments.Format};

    constexpr std::array Bindings{VkVertexInputBindingDescription{0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}};
    constexpr std::array Attrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)},
                               VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)},
                               VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}};

    constexpr VkPushConstantRange PushConstants{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4};

    m_Pipeline->CreateGraphicsPipeline(DeviceModule,
                                       {.Extent = Extent,
                                        .ColorFormats = Formats,
                                        .RenderPass = SwapChainModule->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = std::span{ImGuiVertShaderSpv, sizeof(ImGuiVertShaderSpv) / sizeof(uint32_t)},
                                        .FragmentShader = std::span{ImGuiFragShaderSpv, sizeof(ImGuiFragShaderSpv) / sizeof(uint32_t)},
                                        .SetLayouts = std::array{m_FontSet->GetLayout()},
                                        .Bindings = Bindings,
                                        .Attributes = Attrs,
                                        .PushConstants = std::array{PushConstants},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE});

    return true;
}

void ImGuiBackendLUVK::Shutdown()
{
    if (m_DescPool)
    {
        m_DescPool.reset();
    }

    m_FontSet.reset();
    m_FontImage.reset();
    m_FontSampler.reset();
    m_Pipeline.reset();
    m_VtxBuffer.reset();
    m_IdxBuffer.reset();
}

void ImGuiBackendLUVK::NewFrame() const
{
    ImGui::NewFrame();
}

void ImGuiBackendLUVK::Render(std::shared_ptr<luvk::Memory> const& Memory, const VkCommandBuffer& Cmd)
{
    ImGui::Render();
    ImDrawData* DrawData = ImGui::GetDrawData();

    if (!DrawData || DrawData->TotalVtxCount == 0)
    {
        return;
    }

    const std::size_t VertexSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
    const std::size_t IndexSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if (!m_VtxBuffer || m_VtxBufferSize < VertexSize)
    {
        m_VtxBufferSize = VertexSize;
        if (!m_VtxBuffer)
        {
            m_VtxBuffer = std::make_shared<luvk::Buffer>();
        }
        m_VtxBuffer->CreateBuffer(Memory, {.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, .Size = m_VtxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }

    if (!m_IdxBuffer || m_IdxBufferSize < IndexSize)
    {
        m_IdxBufferSize = IndexSize;
        if (!m_IdxBuffer)
        {
            m_IdxBuffer = std::make_shared<luvk::Buffer>();
        }
        m_IdxBuffer->CreateBuffer(Memory, {.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT, .Size = m_IdxBufferSize, .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }

    std::vector<ImDrawVert> Vertices(DrawData->TotalVtxCount);
    std::vector<ImDrawIdx> Indices(DrawData->TotalIdxCount);

    int VertexOffset = 0;
    int IndexOffset = 0;
    for (int CmdIt = 0; CmdIt < DrawData->CmdListsCount; CmdIt++)
    {
        const ImDrawList* CmdList = DrawData->CmdLists[CmdIt];
        std::memcpy(Vertices.data() + VertexOffset, CmdList->VtxBuffer.Data, CmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        std::memcpy(Indices.data() + IndexOffset, CmdList->IdxBuffer.Data, CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

        VertexOffset += CmdList->VtxBuffer.Size;
        IndexOffset += CmdList->IdxBuffer.Size;
    }

    m_VtxBuffer->Upload(std::as_bytes(std::span{Vertices}));
    m_IdxBuffer->Upload(std::as_bytes(std::span{Indices}));

    const VkBuffer& VertexBufferHandle = m_VtxBuffer->GetHandle();
    const VkBuffer& IndexBufferHandle = m_IdxBuffer->GetHandle();
    constexpr VkDeviceSize Offset = 0;

    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipeline());
    vkCmdBindDescriptorSets(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipelineLayout(), 0, 1, &m_FontSet->GetHandle(), 0, nullptr);

    vkCmdBindVertexBuffers(Cmd, 0, 1, &VertexBufferHandle, &Offset);
    vkCmdBindIndexBuffer(Cmd,
                         IndexBufferHandle,
                         0,
                         sizeof(ImDrawIdx) == 2
                             ? VK_INDEX_TYPE_UINT16
                             : VK_INDEX_TYPE_UINT32);

    struct PC
    {
        float Scale[2];
        float Translate[2];
    } PushConst{};

    PushConst.Scale[0] = 2.0f / DrawData->DisplaySize.x;
    PushConst.Scale[1] = 2.0f / DrawData->DisplaySize.y;
    PushConst.Translate[0] = -1.0f - DrawData->DisplayPos.x * PushConst.Scale[0];
    PushConst.Translate[1] = -1.0f - DrawData->DisplayPos.y * PushConst.Scale[1];
    vkCmdPushConstants(Cmd, m_Pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PC), &PushConst);

    int VertexOf = 0;
    int IndexOf = 0;
    for (int CmdListIt = 0; CmdListIt < DrawData->CmdListsCount; CmdListIt++)
    {
        const ImDrawList* CmdList = DrawData->CmdLists[CmdListIt];
        for (int CmdBufferIt = 0; CmdBufferIt < CmdList->CmdBuffer.Size; CmdBufferIt++)
        {
            const ImDrawCmd* UICmd = &CmdList->CmdBuffer[CmdBufferIt];
            VkRect2D Scissor;
            Scissor.offset.x = static_cast<int>(UICmd->ClipRect.x - DrawData->DisplayPos.x);
            Scissor.offset.y = static_cast<int>(UICmd->ClipRect.y - DrawData->DisplayPos.y);
            Scissor.extent.width = static_cast<uint32_t>(UICmd->ClipRect.z - UICmd->ClipRect.x);
            Scissor.extent.height = static_cast<uint32_t>(UICmd->ClipRect.w - UICmd->ClipRect.y);
            vkCmdSetScissor(Cmd, 0, 1, &Scissor);
            vkCmdDrawIndexed(Cmd, UICmd->ElemCount, 1, UICmd->IdxOffset + IndexOf, static_cast<std::int32_t>(UICmd->VtxOffset) + VertexOf, 0);
        }

        IndexOf += CmdList->IdxBuffer.Size;
        VertexOf += CmdList->VtxBuffer.Size;
    }
}
