// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/ApplicationBase.hpp"
#include <execution>
#include <vector>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Modules/CommandPool.hpp>
#include <luvk/Modules/Debug.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/Renderer.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Modules/Synchronization.hpp>
#include <luvk/Modules/ThreadPool.hpp>
#include <SDL2/SDL_vulkan.h>

using namespace luvk_example;

constexpr luvk::Renderer::InstanceCreationArguments g_InstArguments{
    .ApplicationName = "LuVK Example",
    .EngineName = "luvk",
    .ApplicationVersion = VK_MAKE_VERSION(0U, 0U, 1U),
    .EngineVersion = VK_MAKE_VERSION(0U, 0U, 1U)};

constexpr std::uint32_t g_ImageCount = 3U;

ApplicationBase::ApplicationBase(const std::uint32_t Width, const std::uint32_t Height)
    : m_Width(Width),
      m_Height(Height),
      m_Renderer(luvk::CreateModule<luvk::Renderer>())
{
    volkInitialize();
    SDL_Init(SDL_INIT_EVERYTHING);
    m_Window = SDL_CreateWindow(std::data(g_InstArguments.ApplicationName),
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                static_cast<std::int32_t>(m_Width),
                                static_cast<std::int32_t>(m_Height),
                                SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
}

ApplicationBase::~ApplicationBase()
{
    if (m_DeviceModule)
    {
        m_DeviceModule->WaitIdle();
    }

    if (m_Window)
    {
        SDL_DestroyWindow(m_Window);
    }

    SDL_Quit();
    luvk::FinalizeGlslang();
    volkFinalize();
}

bool ApplicationBase::Initialize()
{
    RegisterModules();
    SetupExtensions();

    if (!m_Renderer->InitializeRenderer(g_InstArguments, nullptr))
    {
        return false;
    }

    if (InitializeModules())
    {
        volkLoadInstance(m_Renderer->GetInstance());
        volkLoadDevice(m_DeviceModule->GetLogicalDevice());
        luvk::InitializeGlslang();
        m_Renderer->InitializeRenderLoop();
        return true;
    }
    return false;
}

bool ApplicationBase::Render()
{
    return false;
}

void ApplicationBase::RegisterModules()
{
    m_DebugModule           = luvk::CreateModule<luvk::Debug>(m_Renderer);
    m_DeviceModule          = luvk::CreateModule<luvk::Device>(m_Renderer);
    m_MemoryModule          = luvk::CreateModule<luvk::Memory>(m_Renderer, m_DeviceModule);
    m_SwapChainModule       = luvk::CreateModule<luvk::SwapChain>(m_DeviceModule, m_MemoryModule);
    m_CommandPoolModule     = luvk::CreateModule<luvk::CommandPool>(m_DeviceModule);
    m_SynchronizationModule = luvk::CreateModule<luvk::Synchronization>(m_DeviceModule, m_SwapChainModule, m_CommandPoolModule, g_ImageCount);
    m_ThreadPoolModule      = luvk::CreateModule<luvk::ThreadPool>();
    m_DescriptorPoolModule  = luvk::CreateModule<luvk::DescriptorPool>(m_DeviceModule);

    m_Renderer->RegisterModules({m_DebugModule, m_DeviceModule, m_MemoryModule, m_SwapChainModule, m_CommandPoolModule, m_SynchronizationModule, m_ThreadPoolModule, m_DescriptorPoolModule});
}

void ApplicationBase::SetupExtensions() const
{
    std::uint32_t NumExtensions = 0U;
    if (SDL_Vulkan_GetInstanceExtensions(m_Window, &NumExtensions, nullptr))
    {
        std::vector<char const*> ExtensionsData(NumExtensions, nullptr);
        SDL_Vulkan_GetInstanceExtensions(m_Window, &NumExtensions, std::data(ExtensionsData));
        for (char const* Ext : ExtensionsData)
        {
            m_Renderer->GetExtensions().SetExtensionState("", Ext, true);
        }
    }
}

bool ApplicationBase::InitializeModules() const
{
    if (!InitializeDeviceModule())
    {
        return false;
    }

    m_MemoryModule->InitializeAllocator(0);
    m_SwapChainModule->CreateSwapChain({.ImageCount = g_ImageCount, .Extent = {m_Width, m_Height}}, nullptr);
    m_CommandPoolModule->CreateCommandPool(m_DeviceModule->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_ThreadPoolModule->Start(std::thread::hardware_concurrency());

    constexpr std::array Sizes = {VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024},
                                  VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024},
                                  VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1024}};

    m_DescriptorPoolModule->CreateDescriptorPool(1000, Sizes);

    m_SynchronizationModule->Initialize();
    m_SynchronizationModule->SetupFrames();
    return true;
}

bool ApplicationBase::InitializeDeviceModule() const
{
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(m_Window, m_Renderer->GetInstance(), &Surface))
    {
        return false;
    }

    m_DeviceModule->SetPhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    m_DeviceModule->SetSurface(Surface);

    auto& DevExt = m_DeviceModule->GetExtensions();
    if (DevExt.HasAvailableExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME))
    {
        DevExt.SetExtensionState("", VK_EXT_MESH_SHADER_EXTENSION_NAME, true);
    }

    if (DevExt.HasAvailableExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME))
    {
        DevExt.SetExtensionState("", VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME, true);
    }

    if (DevExt.HasAvailableExtension(VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME))
    {
        DevExt.SetExtensionState("", VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME, true);
    }

    constexpr VkPhysicalDeviceMeshShaderFeaturesEXT MeshShaderFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
                                                                       .meshShader = VK_TRUE};

    luvk::UnorderedMap<std::uint32_t, std::uint32_t> DeviceQueueMap{};
    const auto&                                      QueueProperties = m_DeviceModule->GetDeviceQueueFamilyProperties();
    std::uint32_t                                    Iterator        = 0U;

    for (const auto& Q : QueueProperties)
    {
        DeviceQueueMap.emplace(Iterator++, Q.queueCount);
    }

    m_DeviceModule->CreateLogicalDevice(std::move(DeviceQueueMap), &MeshShaderFeatures);
    return true;
}
