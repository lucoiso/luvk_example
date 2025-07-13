// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/ImGuiLayer.hpp"
#include <imgui.h>

using namespace luvk_example;

bool ImGuiLayer::Initialize(SDL_Window* Window, std::shared_ptr<luvk::Renderer> const& Renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (!m_SdlBackend.Init(Window))
    {
        return false;
    }
    if (!m_VkBackend.Init(Renderer))
    {
        return false;
    }
    return true;
}

void ImGuiLayer::Shutdown()
{
    m_VkBackend.Shutdown();
    m_SdlBackend.Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::NewFrame(float DeltaTime) const
{
    ImGuiIO& IO = ImGui::GetIO();
    IO.DeltaTime = DeltaTime;
    m_SdlBackend.NewFrame();
    m_VkBackend.NewFrame();
}

void ImGuiLayer::Render(VkCommandBuffer Cmd) const
{
    m_VkBackend.Render(Cmd);
}

bool ImGuiLayer::ProcessEvent(SDL_Event const& Event) const
{
    return m_SdlBackend.ProcessEvent(Event);
}
