// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiLayer.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Types/Array.hpp>

using namespace luvk_example;

constexpr auto g_DefaultFrag = R"(
struct VSOutput {
    float2 fragUV;
};

struct PC {
    float time;
};

[[vk::push_constant]] PC pc;

#define PIXEL_SIZE_FAC 700.0f
#define BLACK (0.6f * float4(79.0f/255.0f, 99.0f/255.0f, 103.0f/255.0f, 1.0f/0.6f))

static const float VortSpeed = 1.0f;
static const float MidFlash = 0.0f;
static const float VortOffset = 0.0f;

static const float4 Colour1 = float4(0.9960784f, 0.3725490f, 0.3333333f, 1.0f);
static const float4 Colour2 = float4(0.0f, 0.6156863f, 1.0f, 1.0f);

float4 Easing(float4 t, float power)
{
    return float4(
       pow(t.x, power),
       pow(t.y, power),
       pow(t.z, power),
       pow(t.w, power)
    );
}

float4 Effect(float2 uv, float scale)
{
    uv = floor(uv * (PIXEL_SIZE_FAC * 0.5f)) / (PIXEL_SIZE_FAC * 0.5f);
    uv /= scale;

    float uvLen = length(uv);

    float speed = pc.time * VortSpeed;

    float angle =
       atan2(uv.y, uv.x)
       + (2.2f + 0.4f * min(6.0f, speed)) * uvLen
       - 1.0f
       - speed * 0.05f
       - min(6.0f, speed) * speed * 0.02f
       + VortOffset;

    float2 sv = float2(
       uvLen * cos(angle),
       uvLen * sin(angle)
    );

    sv *= 30.0f;
    speed = pc.time * 6.0f * VortSpeed + VortOffset + 5.0f;

    float2 uv2 = float2(sv.x + sv.y);

    for (int i = 0; i < 5; ++i)
    {
       uv2 += sin(max(sv.x, sv.y)) + sv;

       sv += 0.5f * float2(
          cos(5.1123314f + 0.353f * uv2.y + speed * 0.131121f),
          sin(uv2.x - 0.113f * speed)
       );

       sv -= 1.0f * cos(sv.x + sv.y)
          - 1.0f * sin(sv.x * 0.711f - sv.y);
    }

    float smoke =
       min(2.0f,
       max(-2.0f,
          1.5f + length(sv) * 0.12f
          - 0.17f * min(10.0f, pc.time * 1.2f)
       ));

    if (smoke < 0.2f)
    {
       smoke = (smoke - 0.2f) * 0.6f + 0.2f;
    }

    float c1p = max(0.0f, 1.0f - 2.0f * abs(1.0f - smoke));
    float c2p = max(0.0f, 1.0f - 2.0f * smoke);
    float cb = 1.0f - min(1.0f, c1p + c2p);

    float4 col =
       Colour1 * c1p +
       Colour2 * c2p +
       float4(cb * BLACK.rgb, cb * Colour1.a);

    float modFlash =
       max(MidFlash * 0.8f, max(c1p, c2p) * 5.0f - 4.4f)
       + MidFlash * max(c1p, c2p);

    return Easing(
       col * (1.0f - modFlash) + modFlash * float4(1.0f),
       1.5f
    );
}

[shader("fragment")]
float4 main(VSOutput input) : SV_Target
{
    float2 uv = input.fragUV * 2.0f - 1.0f;
    uv.x *= 1.0f;
    float4 col = Effect(uv, 2.0f);

    return float4(sqrt(clamp(col.rgb, 0.0f, 1.0f)), col.a);
}
)";

constexpr auto g_DefaultVert = R"(
struct VSOutput {
	float2 fragUV : TEXCOORD0;
	float4 Pos : SV_Position;
};

[shader("vertex")]
VSOutput main(uint gl_VertexIndex : SV_VertexID)
{
	VSOutput output;
	float2 pos = float2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	output.Pos = float4(pos * 2.0f - 1.0f, 0.0f, 1.0f);
	output.fragUV = pos;
	return output;
}
)";

int ImGuiLayer::InputTextCallback(ImGuiInputTextCallbackData* Data)
{
    if (Data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        auto* Str = static_cast<std::string*>(Data->UserData);
        Str->resize(Data->BufTextLen);
        Data->Buf = std::data(*Str);
    }
    return 0;
}

void ImGuiLayer::InitializeEditorResources()
{
    PushStyle();

    m_ShaderCode = g_DefaultFrag;
    m_ShaderCode.reserve(1024 * 4);

    m_PreviewImage = std::make_shared<luvk::Image>(m_Device, m_Memory);
    m_PreviewImage->CreateImage({.Extent = {256, 256, 1},
                                 .Format = VK_FORMAT_R8G8B8A8_UNORM,
                                 .Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                 .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY});

    m_PreviewSampler = std::make_shared<luvk::Sampler>(m_Device);
    m_PreviewSampler->CreateSampler({});

    if (!m_TextureID)
    {
        m_TextureID = std::make_shared<luvk::DescriptorSet>(m_Device, m_Pool, m_Memory);

        constexpr VkDescriptorSetLayoutBinding Binding{0,
                                                       VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                       1,
                                                       VK_SHADER_STAGE_FRAGMENT_BIT,
                                                       nullptr};

        m_TextureID->CreateLayout({.Bindings = luvk::Array{Binding}});
        m_TextureID->Allocate();
    }

    m_TextureID->UpdateImage(m_PreviewImage->GetView(),
                             m_PreviewSampler->GetHandle(),
                             0,
                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    constexpr VkAttachmentDescription Attachment{.format = VK_FORMAT_R8G8B8A8_UNORM,
                                                 .samples = VK_SAMPLE_COUNT_1_BIT,
                                                 .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                 .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                 .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                 .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                 .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                 .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    constexpr VkAttachmentReference Ref{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

    const VkSubpassDescription Subpass{.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                                       .colorAttachmentCount = 1,
                                       .pColorAttachments = &Ref};

    constexpr luvk::Array Dependencies{VkSubpassDependency{.srcSubpass = VK_SUBPASS_EXTERNAL,
                                                           .dstSubpass = 0,
                                                           .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                           .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                           .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                           .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                           .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT},
                                       VkSubpassDependency{.srcSubpass = 0,
                                                           .dstSubpass = VK_SUBPASS_EXTERNAL,
                                                           .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                           .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                           .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                           .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                           .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT}};

    const VkRenderPassCreateInfo RPInfo{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                        .attachmentCount = 1,
                                        .pAttachments = &Attachment,
                                        .subpassCount = 1,
                                        .pSubpasses = &Subpass,
                                        .dependencyCount = static_cast<uint32_t>(std::size(Dependencies)),
                                        .pDependencies = std::data(Dependencies)};

    vkCreateRenderPass(m_Device->GetLogicalDevice(), &RPInfo, nullptr, &m_PreviewRenderPass);

    const VkImageView&            View = m_PreviewImage->GetView();
    const VkFramebufferCreateInfo FBInfo{.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                         .renderPass = m_PreviewRenderPass,
                                         .attachmentCount = 1,
                                         .pAttachments = &View,
                                         .width = 256,
                                         .height = 256,
                                         .layers = 1};

    vkCreateFramebuffer(m_Device->GetLogicalDevice(), &FBInfo, nullptr, &m_PreviewFramebuffer);
    TransitionTextureToReadState();

    m_CachedVertexShader  = luvk::CompileShader(g_DefaultVert);
    m_CachedDefaultShader = luvk::CompileShader(g_DefaultFrag);

    CompileShader();
}

void ImGuiLayer::Draw()
{
    DrawDockspace();
    DrawEditor();
    DrawTexture();
}

void ImGuiLayer::UpdatePreview(const VkCommandBuffer& Cmd)
{
    if (!m_PreviewPipeline)
    {
        return;
    }

    const VkImageMemoryBarrier BarrierToAttachment{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                   .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                   .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                   .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                   .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .image = m_PreviewImage->GetHandle(),
                                                   .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    vkCmdPipelineBarrier(Cmd,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &BarrierToAttachment);

    m_TotalTime += ImGui::GetIO().DeltaTime;

    constexpr VkClearValue      Clear{.color = {0.f, 0.f, 0.f, 1.f}};
    const VkRenderPassBeginInfo Info{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                     .renderPass = m_PreviewRenderPass,
                                     .framebuffer = m_PreviewFramebuffer,
                                     .renderArea = {{0, 0}, {256, 256}},
                                     .clearValueCount = 1,
                                     .pClearValues = &Clear};

    vkCmdBeginRenderPass(Cmd, &Info, VK_SUBPASS_CONTENTS_INLINE);

    constexpr VkViewport VP{0, 0, 256, 256, 0, 1};
    constexpr VkRect2D   Sc{{0, 0}, {256, 256}};

    vkCmdSetViewport(Cmd, 0, 1, &VP);
    vkCmdSetScissor(Cmd, 0, 1, &Sc);
    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PreviewPipeline->GetPipeline());

    vkCmdPushConstants(Cmd,
                       m_PreviewPipeline->GetPipelineLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       0,
                       sizeof(float),
                       &m_TotalTime);

    vkCmdDraw(Cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(Cmd);

    const VkImageMemoryBarrier BarrierToShaderRead{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                   .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                   .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                   .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                   .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .image = m_PreviewImage->GetHandle(),
                                                   .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    vkCmdPipelineBarrier(Cmd,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &BarrierToShaderRead);
}

void ImGuiLayer::DrawDockspace() const
{
    constexpr ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(Viewport->WorkPos);
    ImGui::SetNextWindowSize(Viewport->WorkSize);
    ImGui::SetNextWindowViewport(Viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, WindowFlags);
    ImGui::PopStyleVar(3);

    const ImGuiID DockSpaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(DockSpaceID, ImVec2(0.0f, 0.0f), DockspaceFlags);

    static std::once_flag InitFlag;
    std::call_once(InitFlag,
                   [&]
                   {
                       ImGui::DockBuilderRemoveNode(DockSpaceID);
                       ImGui::DockBuilderAddNode(DockSpaceID, DockspaceFlags | ImGuiDockNodeFlags_DockSpace);
                       ImGui::DockBuilderSetNodeSize(DockSpaceID, ImGui::GetMainViewport()->Size);

                       ImGuiID DockMainID = DockSpaceID;

                       const ImGuiID DockLeftID = ImGui::DockBuilderSplitNode(DockMainID,
                                                                              ImGuiDir_Left,
                                                                              0.30f,
                                                                              nullptr,
                                                                              &DockMainID);

                       ImGuiID DockShaderEditorID = DockLeftID;

                       const ImGuiID DockShaderPreviewID = ImGui::DockBuilderSplitNode(DockShaderEditorID,
                                                                                       ImGuiDir_Down,
                                                                                       0.35f,
                                                                                       nullptr,
                                                                                       &DockShaderEditorID);

                       ImGui::DockBuilderDockWindow("Shader Editor", DockShaderEditorID);
                       ImGui::DockBuilderDockWindow("Shader Preview", DockShaderPreviewID);
                       ImGui::DockBuilderFinish(DockSpaceID);
                   }
                  );

    ImGui::End();
}

void ImGuiLayer::DrawEditor()
{
    ImGui::Begin("Shader Editor", nullptr, ImGuiWindowFlags_NoScrollbar);

    if (ImGui::Button("Compile"))
    {
        CompileShader();
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        m_ShaderCode = g_DefaultFrag;
        CreatePreviewPipeline(m_CachedDefaultShader);
    }

    if (!m_CompileSuccess)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", std::data(m_StatusMessage));
    }

    if (const ImVec2 Avail = ImGui::GetContentRegionAvail();
        Avail.x > 0.0f && Avail.y > 0.0f)
    {
        ImGui::InputTextMultiline("##source",
                                  std::data(m_ShaderCode),
                                  m_ShaderCode.capacity() + 1,
                                  Avail,
                                  ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AllowTabInput,
                                  InputTextCallback,
                                  &m_ShaderCode);
    }

    ImGui::End();
}

void ImGuiLayer::DrawTexture() const
{
    ImGui::Begin("Shader Preview", nullptr, ImGuiWindowFlags_NoScrollbar);

    if (const ImVec2 Avail = ImGui::GetContentRegionAvail();
        m_TextureID && Avail.x > 0.0f && Avail.y > 0.0f)
    {
        ImGui::Image(m_TextureID->GetHandle(), Avail);
    }

    ImGui::End();
}

void ImGuiLayer::PushStyle() const
{
    /* Using: Moonlight style by Madam-Herta from ImThemes */
    ImGuiStyle& ImGuiStyle = ImGui::GetStyle();

    ImGuiStyle.Alpha                            = 1.0F;
    ImGuiStyle.DisabledAlpha                    = 1.0F;
    ImGuiStyle.WindowPadding                    = ImVec2(12.0F, 12.0F);
    ImGuiStyle.WindowRounding                   = 11.5F;
    ImGuiStyle.WindowBorderSize                 = 0.0F;
    ImGuiStyle.WindowMinSize                    = ImVec2(20.0F, 20.0F);
    ImGuiStyle.WindowTitleAlign                 = ImVec2(0.5f, 0.5F);
    ImGuiStyle.WindowMenuButtonPosition         = ImGuiDir_Right;
    ImGuiStyle.ChildRounding                    = 0.0F;
    ImGuiStyle.ChildBorderSize                  = 1.0F;
    ImGuiStyle.PopupRounding                    = 0.0F;
    ImGuiStyle.PopupBorderSize                  = 1.0F;
    ImGuiStyle.FramePadding                     = ImVec2(20.0F, 3.400000095367432F);
    ImGuiStyle.FrameRounding                    = 11.89999961853027F;
    ImGuiStyle.FrameBorderSize                  = 0.0F;
    ImGuiStyle.ItemSpacing                      = ImVec2(4.300000190734863f, 5.5F);
    ImGuiStyle.ItemInnerSpacing                 = ImVec2(7.099999904632568f, 1.799999952316284F);
    ImGuiStyle.CellPadding                      = ImVec2(12.10000038146973f, 9.199999809265137F);
    ImGuiStyle.IndentSpacing                    = 0.0F;
    ImGuiStyle.ColumnsMinSpacing                = 4.900000095367432F;
    ImGuiStyle.ScrollbarSize                    = 11.60000038146973F;
    ImGuiStyle.ScrollbarRounding                = 15.89999961853027F;
    ImGuiStyle.GrabMinSize                      = 3.700000047683716F;
    ImGuiStyle.GrabRounding                     = 20.0F;
    ImGuiStyle.TabRounding                      = 0.0F;
    ImGuiStyle.TabBorderSize                    = 0.0F;
    ImGuiStyle.TabCloseButtonMinWidthSelected   = 0.0F;
    ImGuiStyle.TabCloseButtonMinWidthUnselected = 0.0F;
    ImGuiStyle.ColorButtonPosition              = ImGuiDir_Right;
    ImGuiStyle.ButtonTextAlign                  = ImVec2(0.5f, 0.5F);
    ImGuiStyle.SelectableTextAlign              = ImVec2(0.0F, 0.0F);

    ImVec4* const ImGuiColors = ImGuiStyle.Colors;

    ImGuiColors[ImGuiCol_Text]                  = ImVec4(1.0F, 1.0F, 1.0F, 1.0F);
    ImGuiColors[ImGuiCol_TextDisabled]          = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0F);
    ImGuiColors[ImGuiCol_WindowBg]              = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_ChildBg]               = ImVec4(0.09411764889955521f, 0.1019607856869698f, 0.1176470592617989f, 1.0F);
    ImGuiColors[ImGuiCol_PopupBg]               = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_Border]                = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0F);
    ImGuiColors[ImGuiCol_BorderShadow]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_FrameBg]               = ImVec4(0.1137254908680916f, 0.125490203499794f, 0.1529411822557449f, 1.0F);
    ImGuiColors[ImGuiCol_FrameBgHovered]        = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0F);
    ImGuiColors[ImGuiCol_FrameBgActive]         = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0F);
    ImGuiColors[ImGuiCol_TitleBg]               = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0F);
    ImGuiColors[ImGuiCol_TitleBgActive]         = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0F);
    ImGuiColors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_MenuBarBg]             = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0F);
    ImGuiColors[ImGuiCol_ScrollbarBg]           = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0F);
    ImGuiColors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0F);
    ImGuiColors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_CheckMark]             = ImVec4(0.9725490212440491f, 1.0F, 0.4980392158031464f, 1.0F);
    ImGuiColors[ImGuiCol_SliderGrab]            = ImVec4(0.9725490212440491f, 1.0F, 0.4980392158031464f, 1.0F);
    ImGuiColors[ImGuiCol_SliderGrabActive]      = ImVec4(1.0F, 0.7960784435272217f, 0.4980392158031464f, 1.0F);
    ImGuiColors[ImGuiCol_Button]                = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_ButtonHovered]         = ImVec4(0.1803921610116959f, 0.1882352977991104f, 0.196078434586525f, 1.0F);
    ImGuiColors[ImGuiCol_ButtonActive]          = ImVec4(0.1529411822557449f, 0.1529411822557449f, 0.1529411822557449f, 1.0F);
    ImGuiColors[ImGuiCol_Header]                = ImVec4(0.1411764770746231f, 0.1647058874368668f, 0.2078431397676468f, 1.0F);
    ImGuiColors[ImGuiCol_HeaderHovered]         = ImVec4(0.105882354080677f, 0.105882354080677f, 0.105882354080677f, 1.0F);
    ImGuiColors[ImGuiCol_HeaderActive]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_Separator]             = ImVec4(0.1294117718935013f, 0.1490196138620377f, 0.1921568661928177f, 1.0F);
    ImGuiColors[ImGuiCol_SeparatorHovered]      = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0F);
    ImGuiColors[ImGuiCol_SeparatorActive]       = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0F);
    ImGuiColors[ImGuiCol_ResizeGrip]            = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0F);
    ImGuiColors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.9725490212440491f, 1.0F, 0.4980392158031464f, 1.0F);
    ImGuiColors[ImGuiCol_ResizeGripActive]      = ImVec4(1.0F, 1.0F, 1.0F, 1.0F);
    ImGuiColors[ImGuiCol_Tab]                   = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_TabHovered]            = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_TabActive]             = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_TabUnfocused]          = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0F);
    ImGuiColors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.125490203499794f, 0.2745098173618317f, 0.572549045085907f, 1.0F);
    ImGuiColors[ImGuiCol_PlotLines]             = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0F);
    ImGuiColors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0F);
    ImGuiColors[ImGuiCol_PlotHistogram]         = ImVec4(0.8823529481887817f, 0.7960784435272217f, 0.5607843399047852f, 1.0F);
    ImGuiColors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.95686274766922f, 0.95686274766922f, 0.95686274766922f, 1.0F);
    ImGuiColors[ImGuiCol_TableHeaderBg]         = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0F);
    ImGuiColors[ImGuiCol_TableBorderStrong]     = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0F);
    ImGuiColors[ImGuiCol_TableBorderLight]      = ImVec4(0.0F, 0.0F, 0.0F, 1.0F);
    ImGuiColors[ImGuiCol_TableRowBg]            = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0F);
    ImGuiColors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0F);
    ImGuiColors[ImGuiCol_TextSelectedBg]        = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 1.0F);
    ImGuiColors[ImGuiCol_DragDropTarget]        = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0F, 1.0F);
    ImGuiColors[ImGuiCol_NavHighlight]          = ImVec4(0.2666666805744171f, 0.2901960909366608f, 1.0F, 1.0F);
    ImGuiColors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0F, 1.0F);
    ImGuiColors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176F);
    ImGuiColors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176F);
}

void ImGuiLayer::CompileShader()
{
    if (m_ShaderCode == g_DefaultFrag)
    {
        m_CompileSuccess = true;
        m_StatusMessage.clear();
        CreatePreviewPipeline(m_CachedDefaultShader);
    }
    else
    {
        const auto [Result, Data, Error] = luvk::CompileShaderSafe(m_ShaderCode);
        m_CompileSuccess                 = Result;
        m_StatusMessage                  = Error;

        if (m_CompileSuccess)
        {
            CreatePreviewPipeline(Data);
        }
    }
}

void ImGuiLayer::CreatePreviewPipeline(const luvk::Vector<std::uint32_t>& FragSpirv)
{
    if (m_PreviewPipeline && m_Device)
    {
        m_Device->WaitIdle();
    }

    if (m_PreviewPipeline)
    {
        m_PreviewPipeline.reset();
    }

    m_PreviewPipeline                  = std::make_shared<luvk::Pipeline>(m_Device);
    constexpr luvk::Array ColorFormats = {VK_FORMAT_R8G8B8A8_UNORM};

    constexpr VkPushConstantRange PCRange{.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                                          .offset = 0,
                                          .size = sizeof(float)};

    m_PreviewPipeline->CreateGraphicsPipeline({.Extent = {256, 256},
                                               .ColorFormats = ColorFormats,
                                               .RenderPass = m_PreviewRenderPass,
                                               .Subpass = 0,
                                               .VertexShader = m_CachedVertexShader,
                                               .FragmentShader = FragSpirv,
                                               .PushConstants = luvk::Array{PCRange},
                                               .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                               .CullMode = VK_CULL_MODE_NONE,
                                               .EnableDepthOp = false});
}

void ImGuiLayer::TransitionTextureToReadState() const
{
    constexpr VkImageLayout FromLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    constexpr VkImageLayout ToLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    const VkDevice&     LogicalDevice = m_Device->GetLogicalDevice();
    const std::uint32_t QueueFamily   = m_Device->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value();
    const VkQueue&      Queue         = m_Device->GetQueue(QueueFamily);

    const VkCommandPoolCreateInfo PoolInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                           .pNext = nullptr,
                                           .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
                                           .queueFamilyIndex = QueueFamily};

    VkCommandPool Pool{VK_NULL_HANDLE};
    vkCreateCommandPool(LogicalDevice, &PoolInfo, nullptr, &Pool);

    const VkCommandBufferAllocateInfo AllocationInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                     .pNext = nullptr,
                                                     .commandPool = Pool,
                                                     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                     .commandBufferCount = 1};

    VkCommandBuffer CommandBuffer{VK_NULL_HANDLE};
    vkAllocateCommandBuffers(LogicalDevice, &AllocationInfo, &CommandBuffer);

    constexpr VkCommandBufferBeginInfo CommandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                              .pNext = nullptr,
                                                              .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                              .pInheritanceInfo = nullptr};
    vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);

    VkPipelineStageFlags           SrcStage  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    constexpr VkPipelineStageFlags DstStage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    VkAccessFlags                  SrcAccess = 0;
    constexpr VkAccessFlags        DstAccess = VK_ACCESS_SHADER_READ_BIT;

    const VkImageMemoryBarrier Barrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                       .pNext = nullptr,
                                       .srcAccessMask = SrcAccess,
                                       .dstAccessMask = DstAccess,
                                       .oldLayout = FromLayout,
                                       .newLayout = ToLayout,
                                       .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                       .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                       .image = m_PreviewImage->GetHandle(),
                                       .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    vkCmdPipelineBarrier(CommandBuffer, SrcStage, DstStage, 0, 0, nullptr, 0, nullptr, 1, &Barrier);
    vkEndCommandBuffer(CommandBuffer);

    const VkSubmitInfo QueueSubmitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                       .pNext = nullptr,
                                       .waitSemaphoreCount = 0,
                                       .pWaitSemaphores = nullptr,
                                       .pWaitDstStageMask = nullptr,
                                       .commandBufferCount = 1,
                                       .pCommandBuffers = &CommandBuffer,
                                       .signalSemaphoreCount = 0,
                                       .pSignalSemaphores = nullptr};

    vkQueueSubmit(Queue, 1, &QueueSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Queue);

    vkFreeCommandBuffers(LogicalDevice, Pool, 1, &CommandBuffer);
    vkDestroyCommandPool(LogicalDevice, Pool, nullptr);
}
