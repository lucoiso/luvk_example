/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "Core/Meshes/ImGuiMesh.hpp"
#include <cstring>
#include <luvk/Modules/Device.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Material.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include "luvk/Resources/Texture.hpp"

using namespace Core;

constexpr std::uint32_t g_ImGuiVertexShader[] = {0x07230203,0x00010000,0x00080001,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
                                                 0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
                                                 0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
                                                 0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
                                                 0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
                                                 0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
                                                 0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
                                                 0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
                                                 0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
                                                 0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
                                                 0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
                                                 0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
                                                 0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
                                                 0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
                                                 0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
                                                 0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
                                                 0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
                                                 0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
                                                 0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
                                                 0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
                                                 0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
                                                 0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
                                                 0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
                                                 0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
                                                 0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
                                                 0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
                                                 0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
                                                 0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
                                                 0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
                                                 0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
                                                 0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
                                                 0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
                                                 0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
                                                 0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
                                                 0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
                                                 0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
                                                 0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
                                                 0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
                                                 0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
                                                 0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
                                                 0x0000002d,0x0000002c,0x000100fd,0x00010038};

constexpr std::uint32_t g_ImGuiFragmentShader[] = {0x07230203,0x00010000,0x00080001,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
                                                   0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
                                                   0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
                                                   0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
                                                   0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
                                                   0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
                                                   0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
                                                   0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
                                                   0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
                                                   0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
                                                   0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
                                                   0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
                                                   0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
                                                   0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
                                                   0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
                                                   0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
                                                   0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
                                                   0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
                                                   0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
                                                   0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
                                                   0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
                                                   0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
                                                   0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
                                                   0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
                                                   0x00010038};

ImGuiMesh::ImGuiMesh(luvk::Device* Device, luvk::Memory* Memory, luvk::DescriptorPool* Pool)
    : Mesh(Device, Memory)
{
    constexpr VkDescriptorSetLayoutBinding FontBinding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};
    constexpr VkPushConstantRange          PushConstantRange{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4};

    const auto FontSet = std::make_shared<luvk::DescriptorSet>(m_Device, Pool);
    FontSet->CreateLayout(std::span{&FontBinding, 1});
    FontSet->Allocate();

    const ImGuiIO& IO = ImGui::GetIO();
    unsigned char* Pixels;
    std::int32_t   Width, Height;
    IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

    const auto FontImage = std::make_shared<luvk::Image>(Device, Memory);
    FontImage->CreateImage({.Extent      = {static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1},
                            .Format      = VK_FORMAT_R8G8B8A8_UNORM,
                            .Usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            .Aspect      = VK_IMAGE_ASPECT_COLOR_BIT,
                            .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY,
                            .Name        = "ImGuiFontAtlas"});

    FontImage->Upload(std::as_bytes(std::span{reinterpret_cast<const std::byte*>(Pixels), static_cast<size_t>(Width * Height * 4)}));

    const auto FontSampler = std::make_shared<luvk::Sampler>(Device);
    FontSampler->CreateSampler({.Filter = VK_FILTER_LINEAR, .AddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE});

    FontSet->UpdateImage(0, FontImage->GetView(), FontSampler->GetHandle());

    IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(FontSet->GetHandle()));

    const auto FontPipeline = std::make_shared<luvk::Pipeline>(m_Device);

    constexpr VkFormat   ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
    const std::array     SetLayouts  = {FontSet->GetLayout()};
    constexpr std::array Ranges      = {PushConstantRange};

    const std::vector<VkVertexInputBindingDescription> Bindings = {{0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}};

    const std::vector<VkVertexInputAttributeDescription> Attributes = {{0, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<std::uint32_t>(offsetof(ImDrawVert, pos))},
                                                                       {1, 0, VK_FORMAT_R32G32_SFLOAT, static_cast<std::uint32_t>(offsetof(ImDrawVert, uv))},
                                                                       {2, 0, VK_FORMAT_R8G8B8A8_UNORM, static_cast<std::uint32_t>(offsetof(ImDrawVert, col))}};

    FontPipeline->CreateGraphicsPipeline({.VertexShader     = g_ImGuiVertexShader,
                                          .FragmentShader   = g_ImGuiFragmentShader,
                                          .ColorFormats     = std::span{&ColorFormat, 1},
                                          .Topology         = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                          .CullMode         = VK_CULL_MODE_NONE,
                                          .EnableDepthTest  = false,
                                          .EnableDepthWrite = false,
                                          .SetLayouts       = SetLayouts,
                                          .PushConstants    = Ranges,
                                          .VertexBindings   = Bindings,
                                          .VertexAttributes = Attributes,
                                          .Name             = "ImGuiPipeline"});

    m_Material = std::make_shared<luvk::Material>();
    m_Material->SetPipeline(FontPipeline);
    m_Material->SetTexture(0, std::make_shared<luvk::Texture>(FontImage, FontSampler));
    m_Material->SetDescriptorSet(0, FontSet);
}

ImGuiMesh::ImGuiMesh(luvk::Device* Device, luvk::Memory* Memory, const std::shared_ptr<luvk::Material>& FontMaterial)
    : Mesh(Device, Memory)
{
    m_Material = FontMaterial;
}

void ImGuiMesh::UpdateBuffers(const ImDrawData* const DrawData)
{
    m_ActiveDrawData = nullptr;
    if (!DrawData || DrawData->TotalVtxCount == 0 || DrawData->TotalIdxCount == 0)
    {
        return;
    }

    m_ActiveDrawData = DrawData;

    const size_t TotalVtxSize = DrawData->TotalVtxCount * sizeof(ImDrawVert);
    const size_t TotalIdxSize = DrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if (m_Vertices.size() < static_cast<size_t>(DrawData->TotalVtxCount))
    {
        m_Vertices.resize(DrawData->TotalVtxCount);
    }
    if (m_Indices.size() < static_cast<size_t>(DrawData->TotalIdxCount))
    {
        m_Indices.resize(DrawData->TotalIdxCount);
    }

    auto* VtxDst = m_Vertices.data();
    auto* IdxDst = m_Indices.data();

    for (std::int32_t It = 0; It < DrawData->CmdListsCount; ++It)
    {
        const ImDrawList* CmdList = DrawData->CmdLists[It];
        std::memcpy(VtxDst, CmdList->VtxBuffer.Data, CmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        std::memcpy(IdxDst, CmdList->IdxBuffer.Data, CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        VtxDst += CmdList->VtxBuffer.Size;
        IdxDst += CmdList->IdxBuffer.Size;
    }

    if (!m_VertexBuffer || m_VertexBuffer->GetSize() < TotalVtxSize)
    {
        m_VertexBuffer = std::make_shared<luvk::Buffer>(m_Device, m_Memory);
        m_VertexBuffer->CreateBuffer({.Size               = TotalVtxSize * 2,
                                      .Usage              = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                      .MemoryUsage        = VMA_MEMORY_USAGE_CPU_TO_GPU,
                                      .PersistentlyMapped = true,
                                      .Name               = "ImGuiVtx"});
    }
    m_VertexBuffer->Upload(std::as_bytes(std::span{m_Vertices.data(), static_cast<size_t>(DrawData->TotalVtxCount)}));

    if (!m_IndexBuffer || m_IndexBuffer->GetSize() < TotalIdxSize)
    {
        m_IndexBuffer = std::make_shared<luvk::Buffer>(m_Device, m_Memory);
        m_IndexBuffer->CreateBuffer({.Size               = TotalIdxSize * 2,
                                     .Usage              = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                     .MemoryUsage        = VMA_MEMORY_USAGE_CPU_TO_GPU,
                                     .PersistentlyMapped = true,
                                     .Name               = "ImGuiIdx"});
    }
    m_IndexBuffer->Upload(std::as_bytes(std::span{m_Indices.data(), static_cast<size_t>(DrawData->TotalIdxCount)}));
}

void ImGuiMesh::Render(VkCommandBuffer CommandBuffer) const
{
    if (!m_ActiveDrawData || !m_ActiveDrawData->TotalVtxCount)
    {
        return;
    }

    m_Material->Bind(CommandBuffer);

    const VkBuffer         VtxBuf    = m_VertexBuffer->GetHandle();
    constexpr VkDeviceSize Offsets[] = {0};
    vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &VtxBuf, Offsets);

    vkCmdBindIndexBuffer(CommandBuffer, m_IndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT16);

    float Scale[4];
    Scale[0] = 2.0f / m_ActiveDrawData->DisplaySize.x;
    Scale[1] = 2.0f / m_ActiveDrawData->DisplaySize.y;
    Scale[2] = -1.0f - m_ActiveDrawData->DisplayPos.x * Scale[0];
    Scale[3] = -1.0f - m_ActiveDrawData->DisplayPos.y * Scale[1];

    vkCmdPushConstants(CommandBuffer, m_Material->GetPipeline()->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4, Scale);

    const ImVec2 ClipOff   = m_ActiveDrawData->DisplayPos;
    const ImVec2 ClipScale = m_ActiveDrawData->FramebufferScale;

    std::int32_t GlobalVtxOffset = 0;
    std::int32_t GlobalIdxOffset = 0;

    for (std::int32_t ListIt = 0; ListIt < m_ActiveDrawData->CmdListsCount; ListIt++)
    {
        const ImDrawList* CmdList = m_ActiveDrawData->CmdLists[ListIt];
        for (std::int32_t CmdBufferIt = 0; CmdBufferIt < CmdList->CmdBuffer.Size; CmdBufferIt++)
        {
            const ImDrawCmd* ImGuiCmd = &CmdList->CmdBuffer[CmdBufferIt];

            if (ImGuiCmd->UserCallback)
            {
                ImGuiCmd->UserCallback(CmdList, ImGuiCmd);
            }
            else
            {
                const ImVec2 ClipMin((ImGuiCmd->ClipRect.x - ClipOff.x) * ClipScale.x, (ImGuiCmd->ClipRect.y - ClipOff.y) * ClipScale.y);
                const ImVec2 ClipMax((ImGuiCmd->ClipRect.z - ClipOff.x) * ClipScale.x, (ImGuiCmd->ClipRect.w - ClipOff.y) * ClipScale.y);

                if (ClipMax.x <= ClipMin.x || ClipMax.y <= ClipMin.y)
                {
                    continue;
                }

                VkRect2D Scissor;
                Scissor.offset.x      = std::max(0, static_cast<int32_t>(ClipMin.x));
                Scissor.offset.y      = std::max(0, static_cast<int32_t>(ClipMin.y));
                Scissor.extent.width  = static_cast<uint32_t>(ClipMax.x - ClipMin.x);
                Scissor.extent.height = static_cast<uint32_t>(ClipMax.y - ClipMin.y);
                vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);

                auto DescSet = reinterpret_cast<VkDescriptorSet>(ImGuiCmd->GetTexID());
                vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Material->GetPipeline()->GetLayout(), 0, 1, &DescSet, 0, nullptr);

                vkCmdDrawIndexed(CommandBuffer, ImGuiCmd->ElemCount, 1, ImGuiCmd->IdxOffset + GlobalIdxOffset, ImGuiCmd->VtxOffset + GlobalVtxOffset, 0);
            }
        }

        GlobalIdxOffset += CmdList->IdxBuffer.Size;
        GlobalVtxOffset += CmdList->VtxBuffer.Size;
    }
}
