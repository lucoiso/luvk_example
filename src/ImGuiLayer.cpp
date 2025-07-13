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

    return m_SdlBackend.Init(Window) && m_VkBackend.Init(Renderer);
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

void ImGuiLayer::Render(std::shared_ptr<luvk::Memory> const& Memory, const VkCommandBuffer& Cmd)
{
    m_VkBackend.Render(Memory, Cmd);
}

bool ImGuiLayer::ProcessEvent(SDL_Event const& Event) const
{
    return m_SdlBackend.ProcessEvent(Event);
}
