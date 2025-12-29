/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "Core/Application/ApplicationBase.hpp"
#include <iostream>
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <luvk/Modules/CommandPool.hpp>
#include <luvk/Modules/Debug.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Draw.hpp>
#include <luvk/Modules/Instance.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Modules/Synchronization.hpp>
#include <luvk/Modules/ThreadPool.hpp>

using namespace Core;

constexpr luvk::InstanceCreationArguments g_InstArguments{.ApplicationName = "luvk_example"};

ApplicationBase::ApplicationBase(const std::uint32_t Width, const std::uint32_t Height, const std::uint32_t Flags)
    : m_Width(static_cast<std::int32_t>(Width)),
      m_Height(static_cast<std::int32_t>(Height)),
      m_Renderer(std::make_shared<luvk::Renderer>())
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    m_Title  = g_InstArguments.ApplicationName;
    m_Window = SDL_CreateWindow(std::data(m_Title), m_Width, m_Height, Flags);
    m_Input  = std::make_shared<InputManager>(m_Window);

    SDL_StartTextInput(m_Window);
}

ApplicationBase::~ApplicationBase() = default;

bool ApplicationBase::Initialize()
{
    m_Renderer->RegisterModule<luvk::Instance>();
    m_Renderer->RegisterModule<luvk::Debug>();
    m_Renderer->RegisterModule<luvk::Device>();
    m_Renderer->RegisterModule<luvk::Memory>();
    m_Renderer->RegisterModule<luvk::SwapChain>();
    m_Renderer->RegisterModule<luvk::CommandPool>();
    m_Renderer->RegisterModule<luvk::ThreadPool>();
    m_Renderer->RegisterModule<luvk::DescriptorPool>();
    m_Renderer->RegisterModule<luvk::Synchronization>();
    m_Renderer->RegisterModule<luvk::Draw>();

    UpdateExtensionsAndValidation();

    if (!InitializeRenderingCore())
    {
        return false;
    }

    m_Renderer->InitializeModules();

    const auto SwapChainMod = m_Renderer->GetModule<luvk::SwapChain>();
    SwapChainMod->CreateSwapChain({.PresentMode = VK_PRESENT_MODE_FIFO_KHR, .Extent = {GetWidth(), GetHeight()}, .Surface = m_Surface});

    auto DeleteSurfaceNode = luvk::EventNode::NewNode([this]
                                                      {
                                                          if (const auto* InstanceMod = m_Renderer->GetModule<luvk::Instance>())
                                                          {
                                                              vkDestroySurfaceKHR(InstanceMod->GetHandle(), m_Surface, nullptr);
                                                              m_Surface = VK_NULL_HANDLE;
                                                          }
                                                      },
                                                      true);

    m_SurfaceDeleteHandle = SwapChainMod->GetEventSystem().AddNode(std::move(DeleteSurfaceNode), luvk::ModuleEvents::OnShutdown);

    RegisterInputBindings();
    m_CanRender = true;

    return true;
}

void ApplicationBase::Shutdown()
{
    if (const auto* DeviceMod = m_Renderer->GetModule<luvk::Device>())
    {
        DeviceMod->WaitIdle();
    }

    m_EventHandles.clear();
    m_ImGuiLayer.reset();

    SDL_DestroyWindow(m_Window);
    SDL_Quit();

    m_Renderer->Shutdown();
}

bool ApplicationBase::Render()
{
    m_Input->ProcessEvents();
    if (!m_Input->Running())
    {
        return false;
    }

    if (!m_CanRender)
    {
        SDL_Delay(100);
        return true;
    }

    m_Width  = 0;
    m_Height = 0;
    SDL_GetWindowSizeInPixels(m_Window, &m_Width, &m_Height);

    if (m_ResizePending && m_Width > 0 && m_Height > 0)
    {
        m_Renderer->SetPaused(true);

        if (auto* SwapChain = m_Renderer->GetModule<luvk::SwapChain>())
        {
            SwapChain->Recreate({static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height)});
        }

        m_Renderer->SetPaused(false);

        m_ResizePending = false;
        m_CanRender     = true;
    }

    static auto LastTime    = std::chrono::steady_clock::now();
    const auto  CurrentTime = std::chrono::steady_clock::now();
    const float DeltaTime   = std::chrono::duration<float>(CurrentTime - LastTime).count();

    m_DeltaTime = std::clamp(DeltaTime, 0.0001f, 0.05f);
    LastTime    = CurrentTime;

    m_Renderer->DrawFrame();

    if (m_ImGuiLayer)
    {
        m_ImGuiLayer->RenderPlatformWindows();
    }

    return true;
}

void ApplicationBase::SetTitle(const std::string_view Title) noexcept
{
    m_Title = Title;
    SDL_SetWindowTitle(m_Window, std::data(m_Title));
}

bool ApplicationBase::OnBeginFrame([[maybe_unused]] const VkCommandBuffer CommandBuffer, [[maybe_unused]] const std::uint32_t CurrentFrame)
{
    if (m_ImGuiLayer)
    {
        m_ImGuiLayer->Draw();
    }

    return true;
}

bool ApplicationBase::OnRecordFrame(const VkCommandBuffer CommandBuffer, [[maybe_unused]] const std::uint32_t CurrentFrame)
{
    if (m_ImGuiLayer)
    {
        m_ImGuiLayer->Render(CommandBuffer);
    }

    return true;
}

void ApplicationBase::UserEventCallback(const SDL_Event& Event)
{
    [[maybe_unused]] auto _ = m_ImGuiLayer && m_ImGuiLayer->ProcessEvent(Event);
}

void ApplicationBase::RegisterInputBindings()
{
    m_EventHandles.emplace_back(m_Input->BindEvent(SDL_EVENT_WINDOW_RESIZED,
                                                   [this]([[maybe_unused]] const SDL_Event& Event, const SDL_Window* Window)
                                                   {
                                                       if (Window == m_Window)
                                                       {
                                                           m_ResizePending = true;
                                                       }
                                                   }));

    m_EventHandles.emplace_back(m_Input->BindEvent(SDL_EVENT_WINDOW_MINIMIZED,
                                                   [this]([[maybe_unused]] const SDL_Event& Event, const SDL_Window* Window)
                                                   {
                                                       if (Window == m_Window)
                                                       {
                                                           m_Renderer->SetPaused(true);
                                                           m_CanRender = false;
                                                       }
                                                   }));

    m_EventHandles.emplace_back(m_Input->BindEvent(SDL_EVENT_WINDOW_RESTORED,
                                                   [this]([[maybe_unused]] const SDL_Event& Event, const SDL_Window* Window)
                                                   {
                                                       if (Window == m_Window)
                                                       {
                                                           m_Renderer->SetPaused(false);
                                                           m_CanRender = true;
                                                       }
                                                   }));

    m_EventHandles.emplace_back(m_Input->BindEvent(SDL_EVENT_USER,
                                                   [this](const SDL_Event& Event, [[maybe_unused]] const SDL_Window* Window)
                                                   {
                                                       UserEventCallback(Event);
                                                   }));
}

void ApplicationBase::UpdateExtensionsAndValidation() const
{
    auto* InstanceMod = m_Renderer->GetModule<luvk::Instance>();
    auto* DeviceMod   = m_Renderer->GetModule<luvk::Device>();

    std::uint32_t      NumExtensions     = 0U;
    char const* const* RawExtensionsData = SDL_Vulkan_GetInstanceExtensions(&NumExtensions);

    if (NumExtensions > 0 && RawExtensionsData != nullptr)
    {
        for (std::uint32_t It = 0; It < NumExtensions; ++It)
        {
            InstanceMod->GetExtensions().SetExtensionState("", RawExtensionsData[It], true);
        }
    }

    for (const auto& Module : m_Renderer->GetRegisteredModules())
    {
        if (const auto ExtModule = dynamic_cast<luvk::IExtensionsModule*>(Module))
        {
            for (const auto& [Layer, Exts] : ExtModule->GetInstanceExtensions())
            {
                for (const auto& ExtName : Exts)
                {
                    InstanceMod->GetExtensions().SetExtensionState(Layer, ExtName, true);
                }
            }

            for (const auto& [Layer, Exts] : ExtModule->GetDeviceExtensions())
            {
                for (const auto& ExtName : Exts)
                {
                    DeviceMod->GetExtensions().SetExtensionState(Layer, ExtName, true);
                }
            }
        }
    }
}

bool ApplicationBase::InitializeRenderingCore()
{
    auto* InstanceMod = m_Renderer->GetModule<luvk::Instance>();
    if (!InstanceMod->Initialize(g_InstArguments))
    {
        return false;
    }

    if (!SDL_Vulkan_CreateSurface(m_Window, InstanceMod->GetHandle(), nullptr, &m_Surface))
    {
        std::cerr << "[Application]: Failed to create surface: " << SDL_GetError() << std::endl;
        return false;
    }

    auto* DeviceMod = m_Renderer->GetModule<luvk::Device>();
    DeviceMod->FetchAvailableDevices(InstanceMod->GetHandle());

    DeviceMod->SelectBestPhysicalDevice(m_Surface);

    for (const auto& Module : m_Renderer->GetRegisteredModules())
    {
        if (const auto ExtModule = dynamic_cast<luvk::IExtensionsModule*>(Module))
        {
            for (const auto& [Layer, Exts] : ExtModule->GetDeviceExtensions())
            {
                for (const auto& ExtName : Exts)
                {
                    DeviceMod->GetExtensions().SetExtensionState(Layer, ExtName, true);
                }
            }
        }
    }

    const auto GraphicsFamily = DeviceMod->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

    std::unordered_map<std::uint32_t, std::uint32_t> QueueMap;
    if (GraphicsFamily.has_value())
    {
        QueueMap[GraphicsFamily.value()] = 1;
    }

    DeviceMod->CreateLogicalDevice(std::move(QueueMap));

    auto* DrawMod = m_Renderer->GetModule<luvk::Draw>();

    DrawMod->AddBeginFrameCallback([this](const VkCommandBuffer CommandBuffer, const std::uint32_t FrameIndex)
    {
        OnBeginFrame(CommandBuffer, FrameIndex);
    });

    DrawMod->AddDrawCallback([this](const VkCommandBuffer CommandBuffer, const std::uint32_t FrameIndex)
    {
        OnRecordFrame(CommandBuffer, FrameIndex);
    });

    return true;
}
