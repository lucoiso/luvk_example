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
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Core/Device.hpp>
#include "luvk/Core/Memory.hpp"
#include <luvk/Core/RenderGraph.hpp>
#include <luvk/Core/Debug.hpp>

#pragma region RPS_Example // Will be replaced in future - From: https://gpuopen.com/learn/rps-tutorial/rps-tutorial-part1/#compiling-and-linking-the-rpsl-module
RPS_DECLARE_RPSL_ENTRY(rpsl_triangle, main);

constexpr auto g_Shader = R"(
struct V2P
{
    float4 Pos : SV_Position;
    float4 Color : COLOR0;
};

#define PI (3.14159f)

V2P VSMain(uint vId : SV_VertexID)
{
    float2 pos[3] =
    {
        { -0.5f, -0.5f * tan(PI / 6), },
        {  0.0f,  0.5f / cos(PI / 6), },
        {  0.5f, -0.5f * tan(PI / 6), },
    };

    V2P vsOut;
    vsOut.Pos = float4(pos[min(vId, 2)], 0, 1);
    vsOut.Color = float4(vId == 0 ? 1 : 0, vId == 1 ? 1 : 0, vId == 2 ? 1 : 0, 1);
    return vsOut;
}

float4 PSMain(V2P psIn) : SV_Target0
{
    return psIn.Color;
})";
#pragma endregion RPS_Example // Will be replaced in future - From: https://gpuopen.com/learn/rps-tutorial/rps-tutorial-part1/#compiling-and-linking-the-rpsl-module

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
    constexpr std::uint32_t WindowWidth = 800U;
    constexpr std::uint32_t WindowHeight = 600U;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* const Window = SDL_CreateWindow("LuVK Example",
                                                SDL_WINDOWPOS_CENTERED,
                                                SDL_WINDOWPOS_CENTERED,
                                                WindowWidth,
                                                WindowHeight,
                                                SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    const auto Renderer = std::make_shared<luvk::Renderer>();
    Renderer->PreInitializeRenderer();

    constexpr luvk::Renderer::InstanceCreationArguments InstArguments{.ApplicationName = "LuVK Example",
                                                                      .EngineName = "luvk",
                                                                      .ApplicationVersion = VK_MAKE_VERSION(0U, 0U, 1U),
                                                                      .EngineVersion = VK_MAKE_VERSION(0U, 0U, 1U)};

    // Enable the required extensions
    {
        luvk::InstanceExtensions& InstanceExtensions = Renderer->GetExtensions();

        // Debugging extensions - required for the debugging module
        {
            constexpr auto ValidationLayer = "VK_LAYER_KHRONOS_validation";
            InstanceExtensions.SetLayerState(ValidationLayer, true);
            InstanceExtensions.SetExtensionState(ValidationLayer, VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true);
        }

        // SDL required extensions
        if (std::uint32_t NumExtensions = 0U;
            SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, nullptr))
        {
            if (std::vector<const char*> ExtensionsData(NumExtensions, nullptr);
                SDL_Vulkan_GetInstanceExtensions(Window, &NumExtensions, std::data(ExtensionsData)))
            {
                std::for_each(std::execution::unseq,
                              std::cbegin(ExtensionsData),
                              std::cend(ExtensionsData),
                              [&InstanceExtensions] (const char* const& ExtensionIt)
                              {
                                  InstanceExtensions.SetExtensionState("", ExtensionIt, true);
                              });
            }
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
            auto const DebugModule = std::make_shared<luvk::Debug>();
            auto const DeviceModule = std::make_shared<luvk::Device>();
            auto const MemoryModule = std::make_shared<luvk::Memory>();
            auto const SwapChainModule = std::make_shared<luvk::SwapChain>();
            auto const RenderGraphModule = std::make_shared<luvk::RenderGraph>();

            // Set up the events (optional) -> All set to One Time event (will be removed after executing)
            {
                // Events to call right after initializing the renderer module
                {
                    luvk::EventNode PostInitRendererFlow = luvk::EventNode::NewNode
                    (
                        [&] // Set the physical device and window surface
                        {
                            DeviceModule->SetPhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                            DeviceModule->SetSurface(Surface);
                        },
                        true
                    )
                    .Then
                    (
                        [&] // Configure the device queues and create the logical device
                        {
                            luvk::DeviceExtensions& ExtManager = DeviceModule->GetExtensions();
                            ExtManager.SetExtensionState("", VK_KHR_SWAPCHAIN_EXTENSION_NAME, true);

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
                        },
                        true
                    );

                    luvk::EventGraph& RendererEventsManager = Renderer->GetEventSystem();
                    RendererEventsManager.AddNode(std::move(PostInitRendererFlow), luvk::RendererEvents::OnPostInitialized);
                }

                // Events to call after setting the logical device
                {
                    luvk::EventNode PostDeviceFlow = luvk::EventNode::NewNode
                    (
                        [&] // Configure the memory module
                        {
                            MemoryModule->InitializeAllocator(Renderer,
                                                              DeviceModule,
                                                              VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT);
                        },
                        true
                    )
                    .Then
                    (
                        [&] // Configure the swap chain module
                        {
                            luvk::SwapChain::CreationArguments Args{.ImageCount = 3U,
                                                                    .Extent = VkExtent2D{.width = WindowWidth,
                                                                                         .height = WindowHeight}};

                            SwapChainModule->CreateSwapChain(DeviceModule, std::move(Args), nullptr);
                        },
                        true
                    )
                    .Then
                    (
                        [&] // Configure the Render Graph module // TODO : Replace Render Graph Examples
                        {
                            RenderGraphModule->InitializeRPSDevice(DeviceModule);

                            RenderGraphModule->CreateRenderGraph({RPS_QUEUE_FLAG_GRAPHICS, RPS_QUEUE_FLAG_COMPUTE, RPS_QUEUE_FLAG_COPY},
                                                                 RPS_ENTRY_REF(rpsl_triangle, main));

                            RenderGraphModule->BindNode("Triangle",
                                                        [] ([[maybe_unused]] const RpsCmdCallbackContext* const Context)
                                                        {
                                                            // TODO : Implement rendering callbacks
                                                        },
                                                        nullptr,
                                                        RPS_CMD_CALLBACK_FLAG_NONE);
                        },
                        true
                    );

                    luvk::EventGraph& DeviceEventsManager = DeviceModule->GetEventSystem();
                    DeviceEventsManager.AddNode(std::move(PostDeviceFlow), luvk::DeviceEvents::OnChangedLogicalDevice);
                }
            }

            // Initialize the modules
            // Attention to the order:
            //      Initialization order: First to Last (default)
            //      Shutdown order: Last to First (reverse)
            Renderer->PostInitializeRenderer({DebugModule,
                                              DeviceModule,
                                              MemoryModule,
                                              SwapChainModule,
                                              RenderGraphModule});

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