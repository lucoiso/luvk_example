// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Application.hpp"

#include <SDL2/SDL_vulkan.h>
#include <execution>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

luvk_example::Application::Application(const std::uint32_t Width, const std::uint32_t Height)
    : m_Input(Width, Height),
      m_Width(Width),
      m_Height(Height) {}

luvk_example::Application::~Application()
{
    if (m_DeviceModule)
    {
        m_DeviceModule->WaitIdle();
    }
}

bool luvk_example::Application::Initialize()
{
    SDL_Window* const Window = m_Input.GetWindow();
    m_Renderer = std::make_shared<luvk::Renderer>();
    m_Renderer->PreInitializeRenderer();

    constexpr luvk::Renderer::InstanceCreationArguments InstArguments{.ApplicationName = "LuVK Example",
                                                                      .EngineName = "luvk",
                                                                      .ApplicationVersion = VK_MAKE_VERSION(0U, 0U, 1U),
                                                                      .EngineVersion = VK_MAKE_VERSION(0U, 0U, 1U)};

    m_DebugModule = std::make_shared<luvk::Debug>();
    m_SwapChainModule = std::make_shared<luvk::SwapChain>();
    m_DeviceModule = std::make_shared<luvk::Device>();
    m_MemoryModule = std::make_shared<luvk::Memory>();
    m_CommandPoolModule = std::make_shared<luvk::CommandPool>();
    m_SynchronizationModule = std::make_shared<luvk::Synchronization>();
    m_MeshRegistryModule = std::make_shared<luvk::MeshRegistry>();
    m_ThreadPoolModule = std::make_shared<luvk::ThreadPool>();

    m_Renderer->RegisterModules({m_DebugModule,
                                 m_DeviceModule,
                                 m_SwapChainModule,
                                 m_MemoryModule,
                                 m_CommandPoolModule,
                                 m_SynchronizationModule,
                                 m_MeshRegistryModule,
                                 m_ThreadPoolModule});

    std::uint32_t NumExtensions = 0U;
    if (SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, nullptr))
    {
        std::vector<char const*> ExtensionsData(NumExtensions, nullptr);
        SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, ExtensionsData.data());

        auto& InstExt = m_Renderer->GetExtensions();
        for (char const* Ext : ExtensionsData)
        {
            InstExt.SetExtensionState("", Ext, true);
        }
    }

    if (!m_Renderer->InitializeRenderer(InstArguments, nullptr))
    {
        return false;
    }

    m_Renderer->PostInitializeRenderer();

    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(Window, m_Renderer->GetInstance(), &Surface))
    {
        return false;
    }

    m_DeviceModule->SetPhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    m_DeviceModule->SetSurface(Surface);

    std::unordered_map<std::uint32_t, std::uint32_t> DeviceQueueMap{};
    auto const& QueueProperties = m_DeviceModule->GetDeviceQueueFamilyProperties();
    std::uint32_t Iterator = 0U;

    std::for_each(std::execution::seq,
                  std::cbegin(QueueProperties),
                  std::cend(QueueProperties),
                  [&DeviceQueueMap, &Iterator](VkQueueFamilyProperties const& QueuePropertyIt)
                  {
                      DeviceQueueMap.emplace(Iterator++, QueuePropertyIt.queueCount);
                  });

    m_DeviceModule->CreateLogicalDevice(std::move(DeviceQueueMap), nullptr);

    constexpr std::uint32_t ImageCount = 3U;
    m_SwapChainModule->CreateSwapChain(m_DeviceModule, luvk::SwapChain::CreationArguments{.ImageCount = ImageCount, .Extent = {m_Width, m_Height}}, nullptr);

    const auto GraphicsQueue = m_DeviceModule->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value();
    m_CommandPoolModule->CreateCommandPool(m_DeviceModule, GraphicsQueue, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    m_MemoryModule->InitializeAllocator(m_Renderer, m_DeviceModule, 0);
    m_MeshRegistryModule->Initialize(m_MemoryModule);
    m_ThreadPoolModule->Start(2);

    m_SynchronizationModule->Initialize(m_DeviceModule, ImageCount);
    m_SynchronizationModule->SetupFrames(m_DeviceModule, m_SwapChainModule, m_CommandPoolModule, m_ThreadPoolModule);
    luvk::InitializeGlslang();

    m_Cube = std::make_unique<Cube>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule, m_MemoryModule);
    m_Triangle = std::make_unique<Triangle>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule, m_CommandPoolModule);
    m_Pixel = std::make_unique<Pixel>(m_MeshRegistryModule, m_DeviceModule, m_SwapChainModule);

    m_Renderer->InitializeRenderLoop(m_DeviceModule, m_SwapChainModule, m_CommandPoolModule, m_MeshRegistryModule, m_ThreadPoolModule);
    m_Renderer->GetEventSystem().AddNode(luvk::EventNode::NewNode([]
                                       {
                                           SDL_Log("Render loop initialized");
                                       }),
                                       luvk::RendererEvents::OnRenderLoopInitialized);



    m_Input.BindEvent(SDL_WINDOWEVENT,
                      [&](SDL_Event const& e)
                      {
                          if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                              e.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                          {
                              m_Renderer->SetPaused(true);
                              int NewW = 0, NewH = 0;
                              SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                              m_Renderer->Refresh({static_cast<std::uint32_t>(NewW), static_cast<std::uint32_t>(NewH)});
                              m_Renderer->SetPaused(false);
                          }
                          else if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
                          {
                              m_Renderer->SetPaused(true);
                              m_CanRender = false;
                          }
                          else if (e.window.event == SDL_WINDOWEVENT_RESTORED)
                          {
                              m_Renderer->SetPaused(false);
                              m_CanRender = true;
                          }
                      });

    m_Input.BindEvent(SDL_MOUSEBUTTONDOWN,
                      [&](SDL_Event const& e)
                      {
                          if (e.button.button == SDL_BUTTON_RIGHT)
                          {
                              int W = 0, H = 0;
                              SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                              const glm::vec2 Position{2.F * e.button.x / static_cast<float>(W) - 1.F, 2.F * e.button.y / static_cast<float>(H) - 1.F};
                              m_Triangle->AddInstance(Position);
                          }
                      });

    m_Input.BindEvent(SDL_MOUSEMOTION,
                      [&](SDL_Event const& e)
                      {
                          if (m_Input.LeftHeld())
                          {
                              int W = 0, H = 0;
                              SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                              const glm::vec2 Position{2.F * e.motion.x / static_cast<float>(W) - 1.F, 2.F * e.motion.y / static_cast<float>(H) - 1.F};
                              m_Pixel->AddInstance(Position);
                          }
                      });

    return true;
}

void luvk_example::Application::Run()
{
    SDL_Window* const Window = m_Input.GetWindow();

    auto LastTime = std::chrono::high_resolution_clock::now();
    int Frames = 0;
    auto FpsTime = LastTime;

    while (m_Input.Running())
    {
        m_Input.ProcessEvents();
        if (!m_Input.Running())
        {
            break;
        }

        int CurrentWidth = 0;
        int CurrentHeight = 0;
        SDL_Vulkan_GetDrawableSize(Window, &CurrentWidth, &CurrentHeight);

        if (!m_CanRender)
        {
            continue;
        }

        auto CurrentTime = std::chrono::high_resolution_clock::now();
        float DeltaTime = std::chrono::duration<float>(CurrentTime - LastTime).count();
        LastTime = CurrentTime;

        m_Camera.Update(DeltaTime, m_Input);

        glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(CurrentWidth) / static_cast<float>(CurrentHeight), 0.1F, 10.F);
        Proj[1][1] *= -1.F;

        if (m_Triangle)
        {
            m_Triangle->Update(DeltaTime);
        }

        if (m_Cube)
        {
            m_Cube->Update(DeltaTime, m_Camera.GetViewMatrix(), Proj);
        }

        m_Renderer->DrawFrame();

        ++Frames;
        if (std::chrono::duration<float>(CurrentTime - FpsTime).count() >= 1.F)
        {
            float Fps = static_cast<float>(Frames) / std::chrono::duration<float>(CurrentTime - FpsTime).count();
            std::array<char, 64> Title{};
            std::snprintf(Title.data(), Title.size(), "LuVK Example - %.0f FPS", Fps);
            SDL_SetWindowTitle(Window, Title.data());
            FpsTime = CurrentTime;
            Frames = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    luvk::FinalizeGlslang();
}
