// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "UserInterface/Layers/ImGuiLayer.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <luvk/Modules/Device.hpp>
#include "UserInterface/Application/Application.hpp"

using namespace UserInterface;

ImGuiLayer::ImGuiLayer(SDL_Window*                            Window,
                       std::shared_ptr<luvk::Renderer> const& Renderer)
    : ImGuiLayerBase(Window, Renderer)
{
    const luvk::RenderModules& Modules = Renderer->GetModules();
    InitializeResources(Modules.DeviceModule, Modules.DescriptorPoolModule, Modules.MemoryModule);
}

ImGuiLayer::~ImGuiLayer()
{
    m_ShaderImage.reset();
}

void ImGuiLayer::Draw()
{
    ImGuiLayerBase::Draw();

    const ImGuiID DockID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    static bool FirstTime = true;
    if (FirstTime)
    {
        FirstTime = false;
        ImGui::DockBuilderRemoveNode(DockID);
        ImGui::DockBuilderAddNode(DockID, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(DockID, ImGui::GetMainViewport()->Size);

        ImGuiID CentralID = DockID;
        ImGuiID LeftID    = ImGui::DockBuilderSplitNode(CentralID, ImGuiDir_Left, 0.30f, nullptr, &CentralID);
        ImGuiID DownID    = ImGui::DockBuilderSplitNode(LeftID, ImGuiDir_Down, 0.35f, nullptr, &LeftID);

        ImGui::DockBuilderDockWindow("Shader Editor", LeftID);
        ImGui::DockBuilderDockWindow("Shader Preview", DownID);
        ImGui::DockBuilderFinish(DockID);
    }

    DrawEditor();
    DrawTexture();
}

void ImGuiLayer::UpdatePreview(const VkCommandBuffer Cmd) const
{
    m_ShaderImage->Update(Cmd, Application::GetInstance()->GetDeltaTime());
}

void ImGuiLayer::InitializeResources(std::shared_ptr<luvk::Device> const&         Device,
                                     std::shared_ptr<luvk::DescriptorPool> const& Pool,
                                     std::shared_ptr<luvk::Memory> const&         Memory)
{
    m_ShaderImage = std::make_unique<ShaderImage>(Device, Pool, Memory);
    m_ShaderCode  = ShaderImage::GetDefaultSource();
    m_ShaderCode.reserve(4096);
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
        m_ShaderCode     = ShaderImage::GetDefaultSource();
        m_CompileSuccess = true;
        m_StatusMessage  = "Ready";
        m_ShaderImage->Reset();
    }

    if (!m_CompileSuccess)
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", m_StatusMessage.c_str());
    }

    ImGui::InputTextMultiline("##source",
                              m_ShaderCode.data(),
                              m_ShaderCode.capacity() + 1,
                              ImGui::GetContentRegionAvail(),
                              ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_AllowTabInput,
                              InputTextCallback,
                              &m_ShaderCode);

    ImGui::End();
}

void ImGuiLayer::DrawTexture() const
{
    ImGui::Begin("Shader Preview", nullptr, ImGuiWindowFlags_NoScrollbar);

    if (m_ShaderImage)
    {
        if (const VkDescriptorSet DS = m_ShaderImage->GetDescriptorSet();
            DS != VK_NULL_HANDLE)
        {
            ImGui::Image(DS, ImGui::GetContentRegionAvail());
        }
    }

    ImGui::End();
}

void ImGuiLayer::CompileShader()
{
    m_CompileSuccess = m_ShaderImage->Compile(m_ShaderCode, m_StatusMessage);
}

int ImGuiLayer::InputTextCallback(ImGuiInputTextCallbackData* Data)
{
    if (Data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        auto* Str = static_cast<std::string*>(Data->UserData);
        Str->resize(Data->BufTextLen);
        Data->Buf = Str->data();
    }
    return 0;
}