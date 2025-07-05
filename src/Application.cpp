// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Application.hpp"

#include <SDL2/SDL_vulkan.h>
#include <execution>
#include <vector>
#include <unordered_map>

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

    return true;
}
