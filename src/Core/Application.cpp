// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/Application.hpp"
#include "luvk_example/Core/Camera.hpp"

#include <SDL2/SDL_vulkan.h>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace luvk_example;

Application::Application()
    : ApplicationBase(800, 600)
{
}

Application::~Application()
{
    m_ImGuiLayer.Shutdown();
}

bool Application::Initialize()
{
    if (ApplicationBase::Initialize())
    {
        if (!m_ImGuiLayer.Initialize(m_Window,
                                     m_MeshRegistryModule,
                                     m_DeviceModule,
                                     m_SwapChainModule,
                                     m_MemoryModule))
        {
            return false;
        }

        RegisterMeshes();
        RegisterInputBindings();

        return true;
    }

    return false;
}

Application& Application::GetInstance()
{
    static Application Instance;
    static std::once_flag InitFlag;

    std::call_once(InitFlag,
                   [&]
                   {
                       Instance.Initialize();
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

    std::int32_t CurrentWidth = 0;
    std::int32_t CurrentHeight = 0;
    SDL_Vulkan_GetDrawableSize(m_Window, &CurrentWidth, &CurrentHeight);

    static auto LastTime = std::chrono::high_resolution_clock::now();
    const auto CurrentTime = std::chrono::high_resolution_clock::now();
    const float DeltaTime = std::chrono::duration<float>(CurrentTime - LastTime).count();
    LastTime = CurrentTime;

    m_Camera.Update(DeltaTime, m_Input);

    glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(CurrentWidth) / static_cast<float>(CurrentHeight), 0.1F, 10.F);
    Proj[1][1] *= -1.F;
    m_CubeMesh->Update(DeltaTime, m_Camera.GetViewMatrix(), Proj);
    m_TriangleMesh->Update(DeltaTime);

    if (m_ImGuiLayer.IsInitialized())
    {
        m_ImGuiLayer.NewFrame(DeltaTime);

        m_Renderer->EnqueueCommand([this](const VkCommandBuffer& Cmd)
        {
            m_ImGuiLayer.Render(Cmd);
        });
    }

    m_Renderer->DrawFrame();
    return true;
}

void Application::RegisterMeshes()
{
    m_CubeMesh = std::make_unique<Cube>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule, m_MemoryModule);
    m_TriangleMesh = std::make_unique<Triangle>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule, m_MemoryModule);
    m_PixelMesh = std::make_unique<Pixel>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule);
}

void Application::RegisterInputBindings()
{
    m_Input.BindEvent(SDL_WINDOWEVENT,
                      [&](const SDL_Event& Event)
                      {
                          if (Event.window.event == SDL_WINDOWEVENT_RESIZED ||
                              Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                              Event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                          {
                              m_Renderer->SetPaused(true);
                              {
                                  std::int32_t NewW = 0;
                                  std::int32_t NewH = 0;
                                  SDL_Vulkan_GetDrawableSize(m_Window, &NewW, &NewH);
                                  m_Renderer->Refresh({static_cast<std::uint32_t>(NewW), static_cast<std::uint32_t>(NewH)});
                              }
                              m_Renderer->SetPaused(false);
                          }
                          else if (Event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                          {
                              m_Renderer->SetPaused(true);
                              m_CanRender = false;
                          }
                          else if (Event.window.event == SDL_WINDOWEVENT_RESTORED)
                          {
                              m_Renderer->SetPaused(false);
                              m_CanRender = true;
                          }
                      });

    m_Input.BindEvent(SDL_MOUSEBUTTONDOWN,
                      [&](const SDL_Event& Event)
                      {
                          if (Event.button.button == SDL_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse)
                          {
                              std::int32_t NewW = 0;
                              std::int32_t NewH = 0;
                              SDL_Vulkan_GetDrawableSize(m_Window, &NewW, &NewH);
                              const glm::vec2 Position{2.F * static_cast<float>(Event.button.x) / static_cast<float>(NewW) - 1.F,
                                                       2.F * static_cast<float>(Event.button.y) / static_cast<float>(NewH) - 1.F};

                              m_TriangleMesh->AddInstance(Position);
                          }
                      });

    m_Input.BindEvent(SDL_MOUSEMOTION,
                    [&](const SDL_Event& Event)
                    {
                        if (m_Input.LeftHeld() && !ImGui::GetIO().WantCaptureMouse)
                        {
                            std::int32_t NewW = 0;
                            std::int32_t NewH = 0;
                            SDL_Vulkan_GetDrawableSize(m_Window, &NewW, &NewH);
                            const glm::vec2 Position{2.F * static_cast<float>(Event.motion.x) / static_cast<float>(NewW) - 1.F,
                                                     2.F * static_cast<float>(Event.motion.y) / static_cast<float>(NewH) - 1.F};

                            m_PixelMesh->AddInstance(Position);
                        }
                    });

    m_Input.BindEvent(SDL_USEREVENT,
                    [&](const SDL_Event& Event)
                    {
                        [[maybe_unused]] const auto _ = m_ImGuiLayer.ProcessEvent(Event);
                    });
}
