// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include <cstdint>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#ifdef main
    #undef main
#endif

#include <luvk/Core/Renderer.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/RenderGraph.hpp>

bool DoPollLoop()
{
    SDL_Event WindowEvent;

    while(SDL_PollEvent(&WindowEvent))
    {
        if(WindowEvent.type == SDL_QUIT)
        {
            return false;
        }
    }

    return true;
}

std::int32_t main([[maybe_unused]] std::int32_t argc, [[maybe_unused]] char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* const Window = SDL_CreateWindow("LuVK Example",
                                                SDL_WINDOWPOS_CENTERED,
                                                SDL_WINDOWPOS_CENTERED,
                                                800,
                                                600,
                                                SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    const auto Renderer = std::make_shared<luvk::Renderer>();
    Renderer->PreInitializeRenderer();

    constexpr luvk::Renderer::InstanceCreationArguments InstArguments{.ApplicationName = "LuVK Example",
                                                                      .EngineName = "luvk",
                                                                      .ApplicationVersion = VK_MAKE_VERSION(0U, 0U, 1U),
                                                                      .EngineVersion = VK_MAKE_VERSION(0U, 0U, 1U)};

    // Enable the required extensions
    if (std::uint32_t NumExtensions = 0U;
        SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, nullptr))
    {
        if (std::vector<const char*> ExtensionsData(NumExtensions, nullptr);
            SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, std::data(ExtensionsData)))
        {
            luvk::InstanceExtensions& Extensions = Renderer->GetExtensions();

            std::for_each(std::execution::seq,
                          std::cbegin(ExtensionsData),
                          std::cend(ExtensionsData),
                          [&Extensions] (const char* const& ExtensionIt)
                          {
                              Extensions.SetExtensionState("", ExtensionIt, true);
                          });
        }
    }

    // Initialize the renderer
    if (Renderer->InitializeRenderer(InstArguments, nullptr))
    {
        VkInstance const& VulkanInstance = Renderer->GetInstance();

        // Create the window surface
        if (VkSurfaceKHR Surface;
            SDL_Vulkan_CreateSurface(Window, VulkanInstance, &Surface))
        {
            auto const DeviceModule = std::make_shared<luvk::Device>();
            auto const RenderGraphModule = std::make_shared<luvk::RenderGraph>();

            // Initialize the modules
            Renderer->PostInitializeRenderer({DeviceModule,
                                              RenderGraphModule});

            // Set the physical device and window surface
            {
                DeviceModule->SetPhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                DeviceModule->SetSurface(Surface);
            }

            // Configurate the device queues and create the logical device
            {
                std::unordered_map<std::uint32_t, std::uint32_t> DeviceQueueMap{};
                auto const& QueueProperties = DeviceModule->GetDeviceQueueFamilyProperties();

                std::uint32_t Iterator = 0U;

                std::for_each(std::execution::seq,
                              std::cbegin(QueueProperties),
                              std::cend(QueueProperties),
                              [&DeviceQueueMap, &Iterator] (VkQueueFamilyProperties const& QueuePropertyIt)
                              {
                                  DeviceQueueMap.emplace(Iterator++, QueuePropertyIt.queueCount);
                              });

                DeviceModule->CreateLogicalDevice(std::move(DeviceQueueMap), nullptr);
            }

            // Configurate the Render Graph module
            {
                RenderGraphModule->InitializeRPSDevice(DeviceModule);
            }

            // Application loop
            while (Window && DoPollLoop())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds { 34 });
            }
        }
    }

    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}