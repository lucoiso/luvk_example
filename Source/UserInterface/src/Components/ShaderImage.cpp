/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "UserInterface/Components/ShaderImage.hpp"
#include <luvk/Interfaces/IServiceLocator.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/CommandPool.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/Renderer.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Resources/Texture.hpp>

using namespace UserInterface;

constexpr auto g_ShaderImageDefaultFrag = R"(
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

constexpr auto g_ShaderImageVert = R"(
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

ShaderImage::ShaderImage(const std::shared_ptr<luvk::Renderer>& Renderer)
    : m_Renderer(Renderer)
{
    m_Device = Renderer->GetModule<luvk::Device>();
    m_Pool   = Renderer->GetModule<luvk::DescriptorPool>();
    m_Memory = Renderer->GetModule<luvk::Memory>();
    Initialize();
}

ShaderImage::~ShaderImage()
{
    if (m_Device)
    {
        m_Device->WaitIdle();
    }
}

std::string_view ShaderImage::GetDefaultSource()
{
    return g_ShaderImageDefaultFrag;
}

void ShaderImage::Initialize()
{
    auto Image = std::make_shared<luvk::Image>(m_Device, m_Memory);
    Image->CreateImage({.Extent      = {256, 256, 1},
                        .Format      = VK_FORMAT_R8G8B8A8_UNORM,
                        .Usage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        .MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY,
                        .Name        = "ShaderImageTarget"});

    {
        const auto CmdPool = m_Renderer.lock()->GetModule<luvk::CommandPool>();
        const auto Buffers = CmdPool->Allocate(1);

        constexpr VkCommandBufferBeginInfo BeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                     .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
        vkBeginCommandBuffer(Buffers[0], &BeginInfo);

        VkImageMemoryBarrier2 Barrier{.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                      .srcStageMask     = VK_PIPELINE_STAGE_2_NONE,
                                      .srcAccessMask    = VK_ACCESS_2_NONE,
                                      .dstStageMask     = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                                      .dstAccessMask    = VK_ACCESS_2_SHADER_READ_BIT,
                                      .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
                                      .newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      .image            = Image->GetHandle(),
                                      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

        const VkDependencyInfo DepInfo{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &Barrier};
        vkCmdPipelineBarrier2(Buffers[0], &DepInfo);

        vkEndCommandBuffer(Buffers[0]);

        VkCommandBufferSubmitInfo BufferInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, .commandBuffer = Buffers[0]};
        const VkSubmitInfo2       Submit{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2, .commandBufferInfoCount = 1, .pCommandBufferInfos = &BufferInfo};

        vkQueueSubmit2(m_Device->GetQueue(VK_QUEUE_GRAPHICS_BIT), 1, &Submit, VK_NULL_HANDLE);
        m_Device->WaitQueue(VK_QUEUE_GRAPHICS_BIT);
        CmdPool->Free(Buffers);
    }

    auto Sampler = std::make_shared<luvk::Sampler>(m_Device);
    Sampler->CreateSampler({.Filter = VK_FILTER_LINEAR, .AddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT});

    m_Texture = std::make_shared<luvk::Texture>(Image, Sampler, "ShaderImageTex");

    m_CachedVertexShader = luvk::CompileShader(g_ShaderImageVert);

    VkDescriptorSetLayoutBinding Binding{.binding         = 0,
                                         .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         .descriptorCount = 1,
                                         .stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT};

    m_DescriptorSet = std::make_shared<luvk::DescriptorSet>(m_Device, m_Pool);
    m_DescriptorSet->CreateLayout(std::span{&Binding, 1});
    m_DescriptorSet->Allocate();

    VkDescriptorImageInfo      ImgInfo{.sampler = Sampler->GetHandle(), .imageView = Image->GetView(), .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    const VkWriteDescriptorSet Write{.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                     .dstSet          = m_DescriptorSet->GetHandle(),
                                     .dstBinding      = 0,
                                     .descriptorCount = 1,
                                     .descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                     .pImageInfo      = &ImgInfo};

    vkUpdateDescriptorSets(m_Device->GetLogicalDevice(), 1, &Write, 0, nullptr);

    m_CachedDefaultFragShader = luvk::CompileShader(g_ShaderImageDefaultFrag);
    CreatePipeline(m_CachedDefaultFragShader);
}

void ShaderImage::CreatePipeline(const std::vector<std::uint32_t>& FragSpirv)
{
    m_Pipeline                      = std::make_shared<luvk::Pipeline>(m_Device);
    VkFormat            ColorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkPushConstantRange PCRange{VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float)};

    m_Pipeline->CreateGraphicsPipeline({.VertexShader   = m_CachedVertexShader,
                                        .FragmentShader = FragSpirv,
                                        .ColorFormats   = std::span{&ColorFormat, 1},
                                        .Topology       = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                        .CullMode       = VK_CULL_MODE_NONE,
                                        .PushConstants  = std::span{&PCRange, 1}});
}

bool ShaderImage::Compile(const std::string& Source, std::string& OutError)
{
    const luvk::CompilationResult Result = luvk::CompileShaderSafe(Source);
    if (!Result.Result)
    {
        OutError = Result.Error;
        return false;
    }

    m_Pipeline.reset();
    CreatePipeline(Result.Data);
    OutError = "Compiled Successfully";

    return true;
}

void ShaderImage::Reset()
{
    m_TotalTime = 0.0f;
    m_Pipeline.reset();

    CreatePipeline(m_CachedDefaultFragShader);
}

void ShaderImage::RecordCommands(const VkCommandBuffer Cmd) const
{
    if (!m_Pipeline || !m_Texture)
    {
        return;
    }

    VkImage     Image = m_Texture->GetImage()->GetHandle();
    VkImageView View  = m_Texture->GetImage()->GetView();

    const VkImageMemoryBarrier2 ToAtt{.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                      .srcStageMask     = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                                      .srcAccessMask    = VK_ACCESS_2_SHADER_READ_BIT,
                                      .dstStageMask     = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                      .dstAccessMask    = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                      .oldLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                      .newLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                      .image            = Image,
                                      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    const VkDependencyInfo DepAtt{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &ToAtt};
    vkCmdPipelineBarrier2(Cmd, &DepAtt);

    const VkRenderingAttachmentInfo ColorAtt{.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                             .imageView   = View,
                                             .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                             .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                             .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
                                             .clearValue  = {.color = {0.f, 0.f, 0.f, 1.f}}};

    const VkRenderingInfo RenderInfo{.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                     .renderArea           = {{0, 0}, {256, 256}},
                                     .layerCount           = 1,
                                     .colorAttachmentCount = 1,
                                     .pColorAttachments    = &ColorAtt};

    vkCmdBeginRendering(Cmd, &RenderInfo);

    VkViewport VP{0, 0, 256, 256, 0, 1};
    VkRect2D   Sc{{0, 0}, {256, 256}};
    vkCmdSetViewport(Cmd, 0, 1, &VP);
    vkCmdSetScissor(Cmd, 0, 1, &Sc);

    vkCmdBindPipeline(Cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetHandle());
    vkCmdPushConstants(Cmd, m_Pipeline->GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(float), &m_TotalTime);
    vkCmdDraw(Cmd, 3, 1, 0, 0);

    vkCmdEndRendering(Cmd);

    const VkImageMemoryBarrier2 ToRead{.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                       .srcStageMask     = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                       .srcAccessMask    = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                       .dstStageMask     = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                                       .dstAccessMask    = VK_ACCESS_2_SHADER_READ_BIT,
                                       .oldLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       .newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                       .image            = Image,
                                       .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    const VkDependencyInfo DepRead{.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO, .imageMemoryBarrierCount = 1, .pImageMemoryBarriers = &ToRead};
    vkCmdPipelineBarrier2(Cmd, &DepRead);
}

void ShaderImage::UpdateImmediate(const float DeltaTime)
{
    m_TotalTime += DeltaTime;

    if (auto RendererPtr = m_Renderer.lock())
    {
        m_Device->SubmitImmediate([this](const VkCommandBuffer Cmd)
        {
            RecordCommands(Cmd);
        });
    }
}
