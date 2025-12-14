// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiLayer.hpp"
#include <array>
#include <imgui.h>
#include <imgui_internal.h>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>

using namespace luvk_example;

constexpr auto g_DefaultFrag = R"(
#version 450

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 fragUV;

layout(push_constant) uniform PC {
    float time;
} pc;

void main() {
    vec2 uv = fragUV * 2.0 - 1.0;

    float wave1 = sin(uv.x * 10.0 + pc.time * 2.0);
    float wave2 = cos(uv.y * 8.0 - pc.time * 1.5);
    float wave3 = sin(length(uv) * 12.0 - pc.time * 3.0);

    float angle = pc.time * 0.5;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    vec2 rotUV = rot * uv;

    float spiral = atan(rotUV.y, rotUV.x) + length(rotUV) * 5.0 - pc.time * 2.0;

    float pattern = (wave1 + wave2 + wave3 + sin(spiral)) * 0.25;

    vec3 col1 = vec3(0.5 + 0.5 * sin(pc.time + 0.0),
                     0.5 + 0.5 * sin(pc.time + 2.0),
                     0.5 + 0.5 * sin(pc.time + 4.0));

    vec3 col2 = vec3(0.5 + 0.5 * cos(pc.time + 1.0),
                     0.5 + 0.5 * cos(pc.time + 3.0),
                     0.5 + 0.5 * cos(pc.time + 5.0));

    vec3 finalCol = mix(col1, col2, pattern * 0.5 + 0.5);

    float pulse = 0.8 + 0.2 * sin(pc.time * 4.0);
    finalCol *= pulse;

    outColor = vec4(finalCol, 1.0);
}
)";

constexpr auto g_QuadVert = R"(
#version 450

layout(location = 0) out vec2 fragUV;

void main() {
    vec2 pos = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(pos * 2.f - 1.f, 0.f, 1.f);
    fragUV = pos;
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

        m_TextureID->CreateLayout({.Bindings = std::array{Binding}});
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

    constexpr std::array Dependencies{VkSubpassDependency{.srcSubpass = VK_SUBPASS_EXTERNAL,
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
    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    CompileShader();
}

void ImGuiLayer::Draw()
{
    constexpr bool            OptFullscreen  = true;
    static ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDocking;
    if (OptFullscreen)
    {
        const ImGuiViewport* Viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(Viewport->WorkPos);
        ImGui::SetNextWindowSize(Viewport->WorkSize);
        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, WindowFlags);
    ImGui::PopStyleVar();

    if (OptFullscreen)
    {
        ImGui::PopStyleVar(2);
    }

    const ImGuiID DockSpaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(DockSpaceID, ImVec2(0.0f, 0.0f), DockspaceFlags);

    static bool FirstTime = true;
    if (FirstTime)
    {
        FirstTime = false;

        ImGui::DockBuilderRemoveNode(DockSpaceID);
        ImGui::DockBuilderAddNode(DockSpaceID, DockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(DockSpaceID, ImGui::GetMainViewport()->Size);

        ImGuiID       DockMainID = DockSpaceID;
        const ImGuiID DockLeftID = ImGui::DockBuilderSplitNode(DockMainID, ImGuiDir_Left, 0.30f, nullptr, &DockMainID);

        ImGui::DockBuilderDockWindow("Shader Editor", DockLeftID);
        ImGui::DockBuilderFinish(DockSpaceID);
    }

    ImGui::End();

    ImGui::Begin("Shader Editor", nullptr, ImGuiWindowFlags_NoScrollbar);

    if (ImGui::Button("Compile"))
    {
        CompileShader();
    }

    ImGui::SameLine();

    if (m_CompileSuccess)
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", m_StatusMessage.c_str());
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", m_StatusMessage.c_str());
    }

    ImGui::InputTextMultiline("##source",
                              std::data(m_ShaderCode),
                              m_ShaderCode.capacity() + 1,
                              ImVec2(-FLT_MIN, -260.0f),
                              ImGuiInputTextFlags_CallbackResize,
                              InputTextCallback,
                              &m_ShaderCode);

    if (ImGui::Button("Reset"))
    {
        m_ShaderCode = g_DefaultFrag;
        CompileShader();
    }

    ImGui::Separator();
    ImGui::Text("Preview:");

    if (const ImVec2 Avail = ImGui::GetContentRegionAvail();
        m_TextureID && Avail.x > 0.0f && Avail.y > 0.0f)
    {
        if (Avail.x > 256.0f)
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (Avail.x - 256.0f) * 0.5f);
            ImGui::Image(m_TextureID->GetHandle(), ImVec2(256.0f, 256.0f));
        }
        else
        {
            ImGui::Image(m_TextureID->GetHandle(), ImVec2(Avail.x, Avail.x));
        }
    }

    ImGui::End();
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

void ImGuiLayer::CompileShader()
{
    try
    {
        const auto Spirv = luvk::CompileGLSLToSPIRV(m_ShaderCode, EShLangFragment);
        CreatePreviewPipeline(Spirv);
        m_CompileSuccess = true;
        m_StatusMessage  = "Compiled Successfully";
    }
    catch (const std::exception& e)
    {
        m_CompileSuccess = false;
        m_StatusMessage  = e.what();
        try
        {
            CreatePreviewPipeline(luvk::CompileGLSLToSPIRV(g_DefaultFrag, EShLangFragment));
        }
        catch (...) {}
    }
}

void ImGuiLayer::CreatePreviewPipeline(const std::vector<std::uint32_t>& FragSpirv)
{
    if (m_PreviewPipeline && m_Device)
    {
        m_Device->WaitIdle();
    }

    if (m_PreviewPipeline)
    {
        m_PreviewPipeline.reset();
    }

    m_PreviewPipeline                 = std::make_shared<luvk::Pipeline>(m_Device);
    constexpr std::array ColorFormats = {VK_FORMAT_R8G8B8A8_UNORM};

    constexpr VkPushConstantRange PCRange{.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                                          .offset = 0,
                                          .size = sizeof(float)};

    m_PreviewPipeline->CreateGraphicsPipeline({.Extent = {256, 256},
                                               .ColorFormats = ColorFormats,
                                               .RenderPass = m_PreviewRenderPass,
                                               .Subpass = 0,
                                               .VertexShader = luvk::CompileGLSLToSPIRV(g_QuadVert, EShLangVertex),
                                               .FragmentShader = FragSpirv,
                                               .PushConstants = std::array{PCRange},
                                               .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                               .CullMode = VK_CULL_MODE_NONE,
                                               .EnableDepthOp = false});
}

void ImGuiLayer::TransitionImageLayout(const VkImageLayout OldLayout, const VkImageLayout NewLayout) const
{
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

    if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        SrcStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        SrcAccess = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    const VkImageMemoryBarrier Barrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                       .pNext = nullptr,
                                       .srcAccessMask = SrcAccess,
                                       .dstAccessMask = DstAccess,
                                       .oldLayout = OldLayout,
                                       .newLayout = NewLayout,
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
