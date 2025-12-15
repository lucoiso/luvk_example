// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/Application.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Renderer.hpp>
#include <SDL3/SDL_vulkan.h>

using namespace luvk_example;

Application::Application() : ApplicationBase(800, 600) {}

Application::~Application()
{
    if (m_DeviceModule)
    {
        m_DeviceModule->WaitIdle();
    }

    m_ImGuiLayer.Shutdown();
}

bool Application::Initialize()
{
    if (ApplicationBase::Initialize())
    {
        if (!m_ImGuiLayer.Initialize(m_Window, m_DeviceModule, m_DescriptorPoolModule, m_SwapChainModule, m_MemoryModule))
        {
            return false;
        }

        m_ImGuiLayer.InitializeEditorResources();

        CreateScene();
        RegisterInputBindings();

        m_Renderer->SetPreRenderCallback([this](const VkCommandBuffer& Cmd)
        {
            m_TriangleMesh->Compute(Cmd);
            m_ImGuiLayer.UpdatePreview(Cmd);
        });

        m_Renderer->SetDrawCallback([this](const VkCommandBuffer& Cmd)
        {
            const auto CurrentFrame = static_cast<std::uint32_t>(m_SynchronizationModule->GetCurrentFrame());

            m_CubeMesh->Draw(Cmd, {});
            m_TriangleMesh->Draw(Cmd, {});
            m_PixelMesh->Draw(Cmd, {});
            m_ImGuiLayer.Render(Cmd, CurrentFrame);
        });

        return true;
    }

    return false;
}

Application& Application::GetInstance()
{
    static Application    Instance;
    static std::once_flag InitFlag;
    std::call_once(InitFlag,
                   [&]
                   {
                       if (!Instance.Initialize())
                       {
                           throw std::runtime_error("Failed to initialize");
                       }
                   });
    return Instance;
}

bool Application::Render()
{
    m_Input.ProcessEvents();
    if (!m_Input.Running())
    {
        return false;
    }

    if (!m_CanRender)
    {
        return true;
    }

    std::int32_t Width  = 0;
    std::int32_t Height = 0;
    SDL_GetWindowSizeInPixels(m_Window, &Width, &Height);

    if (m_ResizePending && Width > 0 && Height > 0)
    {
        m_Renderer->SetPaused(true);
        m_Renderer->Refresh({static_cast<std::uint32_t>(Width), static_cast<std::uint32_t>(Height)});
        m_Renderer->SetPaused(false);
        m_ResizePending = false;
        m_CanRender     = true;
    }

    static auto LastTime    = std::chrono::high_resolution_clock::now();
    const auto  CurrentTime = std::chrono::high_resolution_clock::now();
    const float DeltaTime   = std::chrono::duration<float>(CurrentTime - LastTime).count();
    LastTime                = CurrentTime;

    m_Camera.Update(DeltaTime, m_Input);
    glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(Width) / static_cast<float>(Height), 0.1F, 10.F);
    Proj[1][1]     *= -1.F;

    m_CubeMesh->Update(DeltaTime, m_Camera.GetViewMatrix(), Proj);

    if (m_ImGuiLayer.IsInitialized())
    {
        m_ImGuiLayer.NewFrame(DeltaTime);
        m_ImGuiLayer.Draw();
    }

    m_Renderer->DrawFrame();
    return true;
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
                          [[maybe_unused]] auto _ = m_ImGuiLayer.ProcessEvent(Event);
                      });
}
