// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/Backend/ImGuiBackendVulkan.hpp"
#include <cstring>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Types/Array.hpp>
#include <luvk/Types/Material.hpp>
#include <luvk/Types/Mesh.hpp>
#include <luvk/Types/Texture.hpp>

using namespace luvk_example;

constexpr auto g_VertexShader = R"(
struct VSInput {
	[[vk::location(0)]] float2 inPos;
	[[vk::location(1)]] float2 inUV;
	[[vk::location(2)]] float4 inColor;
};

struct PC {
	float2 scale;
	float2 translate;
};
[[vk::push_constant]] PC pc;

[shader("vertex")]
void main(
    VSInput input,
    out float4 Position : SV_POSITION,
    [[vk::location(0)]] out float2 fragUV,
    [[vk::location(1)]] out float4 fragColor)
{
	fragUV = input.inUV;
	fragColor = input.inColor;
	Position = float4(input.inPos * pc.scale + pc.translate, 0.0, 1.0);
}
)";

constexpr auto g_FragmentShader = R"(
struct VSOutput {
	[[vk::location(0)]] float2 fragUV;
	[[vk::location(1)]] float4 fragColor;
};

[[vk::binding(0, 0)]] Sampler2D Font;

[shader("fragment")]
float4 main(VSOutput input) : SV_Target {
	float4 sampledColor = Font.Sample(input.fragUV);
	float4 outColor = input.fragColor * sampledColor;
    return outColor;
}
)";

constexpr VkDescriptorSetLayoutBinding g_FontBinding{
    0,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    1,
    VK_SHADER_STAGE_FRAGMENT_BIT,
    nullptr
};

constexpr luvk::Array g_Bindings{VkVertexInputBindingDescription{0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX}};

constexpr luvk::Array g_Attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos)},
                                   VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv)},
                                   VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col)}};

constexpr VkPushConstantRange g_PushConstantRange{VK_SHADER_STAGE_VERTEX_BIT,
                                                  0,
                                                  sizeof(float) * 4};

ImGuiBackendVulkan::ImGuiBackendVulkan(const std::shared_ptr<luvk::Device>&         Device,
                                       const std::shared_ptr<luvk::DescriptorPool>& Pool,
                                       const std::shared_ptr<luvk::SwapChain>&      Swap,
                                       const std::shared_ptr<luvk::Memory>&         Memory)
    : m_DescPool(Pool),
      m_Device(Device),
      m_SwapChain(Swap),
      m_Memory(Memory)
{
    ImGuiIO& IO            = ImGui::GetIO();
    IO.BackendRendererName = "ImGuiBackendLUVK";
    IO.BackendFlags        |= ImGuiBackendFlags_RendererHasVtxOffset;

    if (!m_FontSet)
    {
        m_FontSet = std::make_shared<luvk::DescriptorSet>(m_Device, m_DescPool, m_Memory);
        m_FontSet->CreateLayout({.Bindings = luvk::Array{g_FontBinding}});
        m_FontSet->Allocate();
    }

    unsigned char* Pixels;
    int            Width, Height;
    IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

    const auto Staging = std::make_shared<luvk::Buffer>(Device, Memory);
    Staging->CreateBuffer({.Name = "ImGui Font Staging",
                           .Size = static_cast<VkDeviceSize>(Width * Height * 4),
                           .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    Staging->Upload({reinterpret_cast<const std::byte*>(Pixels), static_cast<size_t>(Width * Height * 4)});

    auto FontImage = std::make_shared<luvk::Image>(Device, Memory);
    FontImage->CreateImage({.Extent = {static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1},
                            .Format = VK_FORMAT_R8G8B8A8_UNORM,
                            .Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            .Aspect = VK_IMAGE_ASPECT_COLOR_BIT,
                            .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY});

    FontImage->Upload(Staging);

    auto FontSampler = std::make_shared<luvk::Sampler>(Device);
    FontSampler->CreateSampler({.Filter = VK_FILTER_LINEAR,
                                .AddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE});

    m_FontTexture = std::make_shared<luvk::Texture>(FontImage, FontSampler);

    IO.Fonts->SetTexID(reinterpret_cast<ImTextureID>(m_FontSet->GetHandle()));

    m_Pipeline = std::make_shared<luvk::Pipeline>(m_Device);
    m_Pipeline->CreateGraphicsPipeline({.Extent = m_SwapChain->GetExtent(),
                                        .ColorFormats = luvk::Array{m_SwapChain->GetCreationArguments().Format},
                                        .RenderPass = m_SwapChain->GetRenderPass(),
                                        .Subpass = 0,
                                        .VertexShader = luvk::CompileShader(g_VertexShader),
                                        .FragmentShader = luvk::CompileShader(g_FragmentShader),
                                        .SetLayouts = luvk::Array{m_FontSet->GetLayout()},
                                        .Bindings = g_Bindings,
                                        .Attributes = g_Attributes,
                                        .PushConstants = luvk::Array{g_PushConstantRange},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE,
                                        .EnableDepthOp = false});

    const auto Mat = std::make_shared<luvk::Material>();
    Mat->SetPipeline(m_Pipeline);
    Mat->SetDescriptorSet(m_FontSet);
    Mat->SetTexture(m_FontTexture);

    m_Mesh = std::make_shared<luvk::Mesh>(m_Device, m_Memory);
    m_Mesh->SetMaterial(Mat);
}

void ImGuiBackendVulkan::NewFrame() const
{
    ImGuiIO& IO  = ImGui::GetIO();
    IO.DeltaTime = IO.DeltaTime > 0.F
                       ? IO.DeltaTime
                       : 1.F / 60.F;
    ImGui::NewFrame();
}

void ImGuiBackendVulkan::Render(const VkCommandBuffer& Cmd, const std::uint32_t CurrentFrame)
{
    ImGui::Render();

    ImDrawData* RenderData = ImGui::GetDrawData();
    if (!RenderData || RenderData->TotalVtxCount == 0)
    {
        return;
    }

    const auto FbWidth  = static_cast<std::int32_t>(RenderData->DisplaySize.x * RenderData->FramebufferScale.x);
    const auto FbHeight = static_cast<std::int32_t>(RenderData->DisplaySize.y * RenderData->FramebufferScale.y);
    if (FbWidth <= 0 || FbHeight <= 0)
    {
        return;
    }

    m_Vertices.resize(RenderData->TotalVtxCount);
    m_Indices.resize(RenderData->TotalIdxCount);

    std::int32_t VtxOffset = 0;
    std::int32_t IdxOffset = 0;
    for (std::int32_t ListIt = 0; ListIt < RenderData->CmdListsCount; ++ListIt)
    {
        const ImDrawList* CmdList = RenderData->CmdLists[ListIt];
        std::memcpy(m_Vertices.data() + VtxOffset,
                    CmdList->VtxBuffer.Data,
                    CmdList->VtxBuffer.Size * sizeof(ImDrawVert));

        std::memcpy(m_Indices.data() + IdxOffset,
                    CmdList->IdxBuffer.Data,
                    CmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

        VtxOffset += CmdList->VtxBuffer.Size;
        IdxOffset += CmdList->IdxBuffer.Size;
    }

    m_Mesh->UploadVertices(std::as_bytes(std::span{m_Vertices}), RenderData->TotalVtxCount, CurrentFrame);
    m_Mesh->UploadIndices(std::span{m_Indices}, CurrentFrame);

    const auto Material = m_Mesh->GetMaterial();
    Material->Bind(Cmd);

    const luvk::Array Push{2.F / RenderData->DisplaySize.x,
                           2.F / RenderData->DisplaySize.y,
                           -1.F - RenderData->DisplayPos.x * (2.F / RenderData->DisplaySize.x),
                           -1.F - RenderData->DisplayPos.y * (2.F / RenderData->DisplaySize.y)};

    m_Mesh->SetPushConstantData(std::as_bytes(std::span{Push}));

    const VkViewport Viewport{0.F, 0.F, static_cast<float>(FbWidth), static_cast<float>(FbHeight), 0.F, 1.F};
    vkCmdSetViewport(Cmd, 0, 1, &Viewport);

    m_Mesh->Render(Cmd, CurrentFrame);

    const ImVec2 ClipOff   = RenderData->DisplayPos;
    const ImVec2 ClipScale = RenderData->FramebufferScale;

    std::int32_t GlobalVtxOffset = 0;
    std::int32_t GlobalIdxOffset = 0;

    for (std::int32_t ListIt = 0; ListIt < RenderData->CmdListsCount; ++ListIt)
    {
        const ImDrawList* CmdList = RenderData->CmdLists[ListIt];
        for (const ImDrawCmd& CmdIt : CmdList->CmdBuffer)
        {
            if (CmdIt.UserCallback)
            {
                CmdIt.UserCallback(CmdList, &CmdIt);
                continue;
            }

            ImVec2 ClipMin((CmdIt.ClipRect.x - ClipOff.x) * ClipScale.x,
                           (CmdIt.ClipRect.y - ClipOff.y) * ClipScale.y);

            ImVec2 ClipMax((CmdIt.ClipRect.z - ClipOff.x) * ClipScale.x,
                           (CmdIt.ClipRect.w - ClipOff.y) * ClipScale.y);

            ClipMin.x = std::max(ClipMin.x, 0.F);
            ClipMin.y = std::max(ClipMin.y, 0.F);
            ClipMax.x = std::min(ClipMax.x, static_cast<float>(FbWidth));
            ClipMax.y = std::min(ClipMax.y, static_cast<float>(FbHeight));

            if (ClipMax.x <= ClipMin.x || ClipMax.y <= ClipMin.y)
            {
                continue;
            }

            VkRect2D Scissor{{static_cast<int32_t>(ClipMin.x), static_cast<int32_t>(ClipMin.y)},
                             {static_cast<uint32_t>(ClipMax.x - ClipMin.x),
                              static_cast<uint32_t>(ClipMax.y - ClipMin.y)}};
            vkCmdSetScissor(Cmd, 0, 1, &Scissor);

            const auto& DescSet = reinterpret_cast<VkDescriptorSet>(CmdIt.GetTexID());
            vkCmdBindDescriptorSets(Cmd,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_Pipeline->GetPipelineLayout(),
                                    0,
                                    1,
                                    &DescSet,
                                    0,
                                    nullptr);

            vkCmdDrawIndexed(Cmd,
                             CmdIt.ElemCount,
                             1,
                             static_cast<std::int32_t>(CmdIt.IdxOffset) + GlobalIdxOffset,
                             static_cast<std::int32_t>(CmdIt.VtxOffset) + GlobalVtxOffset,
                             0);
        }

        GlobalIdxOffset += CmdList->IdxBuffer.Size;
        GlobalVtxOffset += CmdList->VtxBuffer.Size;
    }

    const VkRect2D FullScissor{{0, 0}, {static_cast<uint32_t>(FbWidth), static_cast<uint32_t>(FbHeight)}};
    vkCmdSetScissor(Cmd, 0, 1, &FullScissor);
}
