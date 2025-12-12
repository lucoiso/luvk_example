// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiLayer.hpp"
#include <imgui.h>

using namespace luvk_example;

bool ImGuiLayer::Initialize(SDL_Window* Window,
                            std::shared_ptr<luvk::MeshRegistry> const& Registry,
                            std::shared_ptr<luvk::Device> const& Device,
                            std::shared_ptr<luvk::SwapChain> const& Swap,
                            std::shared_ptr<luvk::Memory> const& Memory)
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    const bool IsOk = m_SdlBackend.Init(Window);
    m_Mesh = std::make_unique<ImGuiMesh>(Registry, Device, Swap, Memory);
    m_Initialized = IsOk && static_cast<bool>(m_Mesh);

    if (m_Initialized)
    {
        ImGuiIO& IO = ImGui::GetIO();
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    return m_Initialized;
}

void ImGuiLayer::Shutdown()
{
    m_Mesh.reset();
    m_SdlBackend.Shutdown();

    if (m_Initialized)
    {
        ImGui::DestroyContext();
    }

    m_Initialized = false;
}

void ImGuiLayer::NewFrame(const float DeltaTime) const
{
    if (m_Initialized)
    {
        ImGuiIO& IO = ImGui::GetIO();
        IO.DeltaTime = DeltaTime;
        m_SdlBackend.NewFrame();
        if (m_Mesh)
        {
            m_Mesh->NewFrame();
        }

        ImGui::ShowDemoWindow();
    }
}

void ImGuiLayer::Render(const VkCommandBuffer& Cmd) const
{
    if (m_Initialized && m_Mesh)
    {
        m_Mesh->Render(Cmd);
    }
}

bool ImGuiLayer::ProcessEvent(const SDL_Event& Event) const
{
    return m_Initialized && m_SdlBackend.ProcessEvent(Event);
}
