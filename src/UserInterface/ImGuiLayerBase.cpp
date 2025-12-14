// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/ImGuiLayerBase.hpp"
#include <imgui.h>
#include "luvk_example/UserInterface/ImGuiMesh.hpp"

using namespace luvk_example;

ImGuiLayerBase::~ImGuiLayerBase() = default;

bool ImGuiLayerBase::Initialize(SDL_Window*                                  Window,
                                std::shared_ptr<luvk::Device> const&         Device,
                                std::shared_ptr<luvk::DescriptorPool> const& Pool,
                                std::shared_ptr<luvk::SwapChain> const&      Swap,
                                std::shared_ptr<luvk::Memory> const&         Memory)
{
    m_Device = Device;
    m_Pool   = Pool;
    m_Memory = Memory;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& IO    = ImGui::GetIO();
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    const bool IsOk = m_SdlBackend.Init(Window);
    m_Mesh          = std::make_unique<ImGuiMesh>(Device, Pool, Swap, Memory);
    m_Initialized   = IsOk && static_cast<bool>(m_Mesh);

    return m_Initialized;
}

void ImGuiLayerBase::Shutdown()
{
    m_Mesh.reset();
    m_SdlBackend.Shutdown();
    if (m_Initialized)
    {
        ImGui::DestroyContext();
    }
    m_Initialized = false;
}

void ImGuiLayerBase::NewFrame(const float DeltaTime) const
{
    if (m_Initialized)
    {
        ImGuiIO& IO  = ImGui::GetIO();
        IO.DeltaTime = DeltaTime;
        m_SdlBackend.NewFrame();
        if (m_Mesh)
        {
            m_Mesh->NewFrame();
        }
    }
}

void ImGuiLayerBase::Render(const VkCommandBuffer& Cmd, std::uint32_t CurrentFrame) const
{
    if (m_Initialized && m_Mesh)
    {
        m_Mesh->Render(Cmd, CurrentFrame);
    }
}

bool ImGuiLayerBase::ProcessEvent(const SDL_Event& Event) const
{
    return m_Initialized && m_SdlBackend.ProcessEvent(Event);
}

void ImGuiLayerBase::Draw()
{
    ImGui::ShowDemoWindow();
}
