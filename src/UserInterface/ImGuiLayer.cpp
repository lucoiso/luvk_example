// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiLayer.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <luvk/Modules/Device.hpp>
#include "luvk_example/Application/Application.hpp"

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

ImGuiLayer::ImGuiLayer(SDL_Window*                                  Window,
                       const VkInstance                             Instance,
                       std::shared_ptr<luvk::Device> const&         Device,
                       std::shared_ptr<luvk::DescriptorPool> const& Pool,
                       std::shared_ptr<luvk::SwapChain> const&      Swap,
                       std::shared_ptr<luvk::Memory> const&         Memory)
    : ImGuiLayerBase(Window,
                     Instance,
                     Device,
                     Pool,
                     Swap,
                     Memory)
{
    InitializeResources(Device, Pool, Memory);
}

ImGuiLayer::~ImGuiLayer()
{
    m_ShaderImage.reset();
}

void ImGuiLayer::Draw()
{
    ImGuiLayerBase::Draw();

    DrawDockspace();
    DrawEditor();
    DrawTexture();
}

void ImGuiLayer::UpdatePreview(const VkCommandBuffer Cmd)
{
    m_ShaderImage->Update(Cmd, Application::GetInstance().GetDeltaTime());
}

void ImGuiLayer::InitializeResources(std::shared_ptr<luvk::Device> const&         Device,
                                     std::shared_ptr<luvk::DescriptorPool> const& Pool,
                                     std::shared_ptr<luvk::Memory> const&         Memory)
{
    m_ShaderCode = g_DefaultFrag;
    m_ShaderCode.reserve(1024 * 4);

    m_ShaderImage = std::make_unique<ShaderImage>(Device, Pool, Memory);
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
        m_ShaderImage->Reset();
        m_CompileSuccess = true;
        m_StatusMessage  = "Ready";
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
        m_ShaderImage && Avail.x > 0.0f && Avail.y > 0.0f)
    {
        const VkDescriptorSet DS = m_ShaderImage->GetDescriptorSet();
        if (DS != VK_NULL_HANDLE)
        {
            ImGui::Image(DS, Avail);
        }
    }

    ImGui::End();
}

void ImGuiLayer::CompileShader()
{
    if (m_ShaderCode == g_DefaultFrag)
    {
        m_CompileSuccess = true;
        m_StatusMessage.clear();
        m_ShaderImage->Reset();
    }
    else
    {
        m_CompileSuccess = m_ShaderImage->Compile(m_ShaderCode, m_StatusMessage);
    }
}

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
