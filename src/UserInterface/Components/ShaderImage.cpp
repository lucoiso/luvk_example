// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/Components/ShaderImage.hpp"
#include <array>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Types/Texture.hpp>

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
[shader("vertex")]
void main(
    uint gl_VertexIndex : SV_VertexID,
    out float4 Pos : SV_Position,
    [[vk::location(0)]] out float2 fragUV)
{
	float2 pos = float2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	Pos = float4(pos * 2.0f - 1.0f, 0.0f, 1.0f);
	fragUV = pos;
}
)";

ShaderImage::ShaderImage(std::shared_ptr<luvk::Device>         Device,
                         std::shared_ptr<luvk::DescriptorPool> Pool,
                         std::shared_ptr<luvk::Memory>         Memory)
    : m_Device(std::move(Device)),
      m_Pool(std::move(Pool)),
      m_Memory(std::move(Memory))
{
    Initialize();
}

ShaderImage::~ShaderImage()
{
    if (m_Device)
    {
        const VkDevice LogicalDevice = m_Device->GetLogicalDevice();
        if (m_Framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(LogicalDevice, m_Framebuffer, nullptr);
        }
        if (m_RenderPass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(LogicalDevice, m_RenderPass, nullptr);
        }
    }
}

void ShaderImage::Update(const VkCommandBuffer Cmd, const float DeltaTime)
{
    if (!m_Pipeline || !m_Texture)
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
                                                   .image = m_Texture->GetImage()->GetHandle(),
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

    m_TotalTime += DeltaTime;

    constexpr VkClearValue      Clear{.color = {0.f, 0.f, 0.f, 1.f}};
    const VkRenderPassBeginInfo Info{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                     .renderPass = m_RenderPass,
                                     .framebuffer = m_Framebuffer,
                                     .renderArea = {{0, 0}, {256, 256}},
                                     .clearValueCount = 1,
                                     .pClearValues = &Clear};

    vkCmdBeginRenderPass(Cmd, &Info, VK_SUBPASS_CONTENTS_INLINE);

    constexpr VkViewport VP{0, 0, 256, 256, 0, 1};
    constexpr VkRect2D   Sc{{0, 0}, {256, 256}};

    vkCmdSetViewport(Cmd, 0, 1, &VP);
    vkCmdSetScissor(Cmd, 0, 1, &Sc);
    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipeline());

    vkCmdPushConstants(Cmd,
                       m_Pipeline->GetPipelineLayout(),
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
                                                   .image = m_Texture->GetImage()->GetHandle(),
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

bool ShaderImage::Compile(const std::string& Source, std::string& OutError)
{
    const auto [Result, Data, Error] = luvk::CompileShaderSafe(Source);

    if (Result)
    {
        m_TotalTime = 0.f;
        CreatePipeline(Data);
        return true;
    }

    OutError = Error;
    return false;
}

void ShaderImage::Reset()
{
    std::string           DummyError;
    [[maybe_unused]] auto _ = Compile(g_DefaultFrag, DummyError);
}

VkDescriptorSet ShaderImage::GetDescriptorSet() const
{
    return m_DescriptorSet
               ? m_DescriptorSet->GetHandle()
               : VK_NULL_HANDLE;
}

void ShaderImage::Initialize()
{
    m_CachedVertexShader = luvk::CompileShader(g_DefaultVert);

    auto PreviewImage = std::make_shared<luvk::Image>(m_Device, m_Memory);
    PreviewImage->CreateImage({.Extent = {256, 256, 1},
                               .Format = VK_FORMAT_R8G8B8A8_UNORM,
                               .Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                               .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY});

    auto PreviewSampler = std::make_shared<luvk::Sampler>(m_Device);
    PreviewSampler->CreateSampler({});

    m_Texture = std::make_shared<luvk::Texture>(PreviewImage, PreviewSampler);

    m_DescriptorSet = std::make_shared<luvk::DescriptorSet>(m_Device, m_Pool, m_Memory);

    constexpr VkDescriptorSetLayoutBinding Binding{0,
                                                   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                   1,
                                                   VK_SHADER_STAGE_FRAGMENT_BIT,
                                                   nullptr};

    m_DescriptorSet->CreateLayout({.Bindings = std::array{Binding}});
    m_DescriptorSet->Allocate();

    m_DescriptorSet->UpdateImage(m_Texture->GetImage()->GetView(),
                                 m_Texture->GetSampler()->GetHandle(),
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

    vkCreateRenderPass(m_Device->GetLogicalDevice(), &RPInfo, nullptr, &m_RenderPass);

    const VkImageView             View = m_Texture->GetImage()->GetView();
    const VkFramebufferCreateInfo FBInfo{.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                         .renderPass = m_RenderPass,
                                         .attachmentCount = 1,
                                         .pAttachments = &View,
                                         .width = 256,
                                         .height = 256,
                                         .layers = 1};

    vkCreateFramebuffer(m_Device->GetLogicalDevice(), &FBInfo, nullptr, &m_Framebuffer);
    TransitionLayout();
    Reset();
}

void ShaderImage::CreatePipeline(const std::vector<std::uint32_t>& FragSpirv)
{
    if (m_Pipeline)
    {
        m_Device->WaitIdle();
        m_Pipeline.reset();
    }

    m_Pipeline                        = std::make_shared<luvk::Pipeline>(m_Device);
    constexpr std::array ColorFormats = {VK_FORMAT_R8G8B8A8_UNORM};

    constexpr VkPushConstantRange PCRange{.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                                          .offset = 0,
                                          .size = sizeof(float)};

    m_Pipeline->CreateGraphicsPipeline({.Extent = {256, 256},
                                        .ColorFormats = ColorFormats,
                                        .RenderPass = m_RenderPass,
                                        .Subpass = 0,
                                        .VertexShader = m_CachedVertexShader,
                                        .FragmentShader = FragSpirv,
                                        .PushConstants = std::array{PCRange},
                                        .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode = VK_CULL_MODE_NONE,
                                        .EnableDepthOp = false});
}

void ShaderImage::TransitionLayout() const
{
    constexpr VkImageLayout FromLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    constexpr VkImageLayout ToLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    const VkDevice      LogicalDevice = m_Device->GetLogicalDevice();
    const std::uint32_t QueueFamily   = m_Device->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value();
    const VkQueue       Queue         = m_Device->GetQueue(QueueFamily);

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
                                       .image = m_Texture->GetImage()->GetHandle(),
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
