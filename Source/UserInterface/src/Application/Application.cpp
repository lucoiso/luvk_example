/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "UserInterface/Application/Application.hpp"
#include <imgui.h>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include "UserInterface/Components/Camera.hpp"
#include "UserInterface/Layers/ImGuiLayer.hpp"
#include "UserInterface/Meshes/Cube.hpp"
#include "UserInterface/Meshes/Pixel.hpp"
#include "UserInterface/Meshes/Triangle.hpp"

using namespace UserInterface;

std::shared_ptr<Application> Application::s_Instance = nullptr;

Application::Application(const std::uint32_t Width, const std::uint32_t Height)
    : ApplicationBase(Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE),
      m_Camera(std::make_shared<Camera>(m_Input)) {}

bool Application::Initialize()
{
    luvk::InitializeShaderCompiler();

    if (ApplicationBase::Initialize())
    {
        CreateScene();
        return true;
    }
    return false;
}

void Application::Shutdown()
{
    if (const auto* Device = m_Renderer->GetModule<luvk::Device>())
    {
        Device->WaitIdle();
    }

    m_CubeMesh.reset();
    m_TriangleMesh.reset();
    m_PixelMesh.reset();
    m_Camera.reset();

    luvk::ShutdownShaderCompiler();
    ApplicationBase::Shutdown();

    s_Instance.reset();
}

bool Application::Render()
{
    if (auto* Layer = dynamic_cast<ImGuiLayer*>(m_ImGuiLayer.get()))
    {
        Layer->ExecutePendingCompile();
        Layer->UpdatePreviewImmediate(m_DeltaTime);
    }

    return ApplicationBase::Render();
}

std::shared_ptr<Application> Application::GetInstance()
{
    if (!s_Instance)
    {
        s_Instance = std::shared_ptr<Application>(new Application(800, 600));
    }
    return s_Instance;
}

void Application::CreateScene()
{
    m_CubeMesh = std::make_unique<Cube>(m_Renderer->GetModule<luvk::Device>(), m_Renderer->GetModule<luvk::SwapChain>(), m_Renderer->GetModule<luvk::Memory>());

    m_TriangleMesh = std::make_unique<Triangle>(m_Renderer->GetModule<luvk::Device>(),
                                                m_Renderer->GetModule<luvk::SwapChain>(),
                                                m_Renderer->GetModule<luvk::Memory>(),
                                                m_Renderer->GetModule<luvk::DescriptorPool>());

    m_PixelMesh = std::make_unique<Pixel>(m_Renderer->GetModule<luvk::Device>(),
                                          m_Renderer->GetModule<luvk::SwapChain>(),
                                          m_Renderer->GetModule<luvk::Memory>());
}

bool Application::OnBeginFrame(const VkCommandBuffer CommandBuffer, const std::uint32_t CurrentFrame)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        m_Camera->Update(m_DeltaTime);
    }

    m_CubeMesh->Tick(m_DeltaTime);
    m_TriangleMesh->Tick(m_DeltaTime);
    m_TriangleMesh->Compute(CommandBuffer);

    if (const auto* Layer = dynamic_cast<ImGuiLayer*>(m_ImGuiLayer.get()))
    {
        Layer->RecordCommands(CommandBuffer);
    }

    return ApplicationBase::OnBeginFrame(CommandBuffer, CurrentFrame);
}

bool Application::OnRecordFrame(const VkCommandBuffer CommandBuffer, const std::uint32_t CurrentFrame)
{
    m_CubeMesh->Render(CommandBuffer);
    m_TriangleMesh->Render(CommandBuffer);
    m_PixelMesh->Render(CommandBuffer);

    return ApplicationBase::OnRecordFrame(CommandBuffer, CurrentFrame);
}

void Application::UserEventCallback(const SDL_Event& Event)
{
    ApplicationBase::UserEventCallback(Event);

    if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && Event.button.button == SDL_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse)
    {
        std::int32_t NewW = 0, NewH = 0;
        SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);
        const glm::vec2 Position{2.F * Event.button.x / static_cast<float>(NewW) - 1.F, 2.F * Event.button.y / static_cast<float>(NewH) - 1.F};
        m_TriangleMesh->AddInstance(Position);
    }

    if (Event.type == SDL_EVENT_MOUSE_MOTION && m_Input->LeftHeld() && !ImGui::GetIO().WantCaptureMouse)
    {
        std::int32_t NewW = 0, NewH = 0;
        SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);
        const glm::vec2 Position{2.F * Event.motion.x / static_cast<float>(NewW) - 1.F, 2.F * Event.motion.y / static_cast<float>(NewH) - 1.F};
        m_PixelMesh->AddInstance(Position);
    }
}
