// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Application/Application.hpp"
#include <chrono>
#include <imgui.h>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Renderer.hpp>
#include <luvk/Modules/Synchronization.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_vulkan.h>
#include "luvk_example/Meshes/Cube.hpp"
#include "luvk_example/Meshes/Pixel.hpp"
#include "luvk_example/Meshes/Triangle.hpp"
#include "luvk_example/UserInterface/ImGuiLayer.hpp"

using namespace luvk_example;

Application::Application(const std::uint32_t Width, const std::uint32_t Height)
    : ApplicationBase(Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE)
{
    ApplicationBase::Initialize();

    const std::string InitTitle = m_Title;

    m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Window,
                                                m_DeviceModule,
                                                m_DescriptorPoolModule,
                                                m_SwapChainModule,
                                                m_MemoryModule);

    CreateScene();
    RegisterInputBindings();

    m_Renderer->SetPreRenderCallback([this, InitTitle](const VkCommandBuffer& Cmd)
    {
        {
            static float        Interval   = 0.f;
            static std::int32_t FrameCount = 0;

            Interval += m_DeltaTime;
            FrameCount++;

            if (Interval > 1.f)
            {
                const float FPS         = static_cast<float>(FrameCount) / Interval;
                const float DeltaTimeMs = (Interval / static_cast<float>(FrameCount)) * 1000.f;

                SetTitle(std::format("{} - {:.2f} FPS ({:.2f} ms)", InitTitle, FPS, DeltaTimeMs));

                Interval   = 0.f;
                FrameCount = 0;
            }
        }

        if (!ImGui::GetIO().WantCaptureMouse)
        {
            m_Camera.Update(m_DeltaTime, m_Input);
        }

        m_CubeMesh->Tick(m_DeltaTime);
        m_TriangleMesh->Tick(m_DeltaTime);
        m_PixelMesh->Tick(m_DeltaTime);

        m_ImGuiLayer->Draw();
        m_ImGuiLayer->UpdatePreview(Cmd);

        m_TriangleMesh->Compute(Cmd);
    });

    m_Renderer->SetDrawCallback([this](const VkCommandBuffer& Cmd)
    {
        const auto FrameIndex = static_cast<std::uint32_t>(m_SynchronizationModule->GetCurrentFrame());
        m_CubeMesh->Render(Cmd, FrameIndex);
        m_TriangleMesh->Render(Cmd, FrameIndex);
        m_PixelMesh->Render(Cmd, FrameIndex);

        m_ImGuiLayer->Render(Cmd, FrameIndex);
    });
}

Application::~Application()
{
    m_DeviceModule->WaitIdle();

    m_CubeMesh.reset();
    m_TriangleMesh.reset();
    m_PixelMesh.reset();
    m_ImGuiLayer.reset();
}

Application& Application::GetInstance()
{
    static Application Instance(800, 600);
    return Instance;
}

void Application::CreateScene()
{
    m_CubeMesh     = std::make_unique<Cube>(m_DeviceModule, m_SwapChainModule, m_MemoryModule, m_DescriptorPoolModule);
    m_TriangleMesh = std::make_unique<Triangle>(m_DeviceModule, m_SwapChainModule, m_MemoryModule, m_DescriptorPoolModule);
    m_PixelMesh    = std::make_unique<Pixel>(m_DeviceModule, m_SwapChainModule, m_MemoryModule);
}

void Application::RegisterInputBindings()
{
    m_Input.BindEvent(SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED,
                      [&]([[maybe_unused]] const SDL_Event& Event)
                      {
                          m_ResizePending = true;
                      });

    m_Input.BindEvent(SDL_EVENT_WINDOW_MINIMIZED,
                      [&]([[maybe_unused]] const SDL_Event& Event)
                      {
                          m_Renderer->SetPaused(true);
                          m_CanRender = false;
                      });

    m_Input.BindEvent(SDL_EVENT_WINDOW_RESTORED,
                      [&]([[maybe_unused]] const SDL_Event& Event)
                      {
                          m_Renderer->SetPaused(false);
                          m_CanRender = true;
                      });

    m_Input.BindEvent(SDL_EVENT_MOUSE_BUTTON_DOWN,
                      [&](const SDL_Event& Event)
                      {
                          if (Event.button.button == SDL_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse)
                          {
                              std::int32_t NewW = 0;
                              std::int32_t NewH = 0;
                              SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);

                              const glm::vec2 Position{2.F * Event.button.x / static_cast<float>(NewW) - 1.F,
                                                       2.F * Event.button.y / static_cast<float>(NewH) - 1.F};

                              m_TriangleMesh->AddInstance(Position);
                          }
                      });

    m_Input.BindEvent(SDL_EVENT_MOUSE_MOTION,
                      [&](const SDL_Event& Event)
                      {
                          if (m_Input.LeftHeld() && !ImGui::GetIO().WantCaptureMouse)
                          {
                              std::int32_t NewW = 0;
                              std::int32_t NewH = 0;
                              SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);

                              const glm::vec2 Position{2.F * Event.motion.x / static_cast<float>(NewW) - 1.F,
                                                       2.F * Event.motion.y / static_cast<float>(NewH) - 1.F};

                              m_PixelMesh->AddInstance(Position);
                          }
                      });

    m_Input.BindEvent(SDL_EVENT_USER,
                      [&](const SDL_Event& Event)
                      {
                          [[maybe_unused]] auto _ = m_ImGuiLayer && m_ImGuiLayer->ProcessEvent(Event);
                      });
}
