// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include <SDL2/SDL_vulkan.h>
#include "luvk_example/Core/Application.hpp"
#include "luvk_example/Core/Camera.hpp"
#include "luvk_example/Meshes/Cube.hpp"
#include "luvk_example/Meshes/Pixel.hpp"
#include "luvk_example/Meshes/Triangle.hpp"
#include "luvk_example/UserInterface/ImGuiLayer.hpp"

#ifdef main
#undef main
#endif

#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>
#include <luvk/Types/ScopeCaller.hpp>

using namespace luvk_example;

std::int32_t main()
{
    try
    {
        constexpr std::uint32_t WindowWidth = 800U;
        constexpr std::uint32_t WindowHeight = 600U;

        Application App(WindowWidth, WindowHeight);
        if (!App.Initialize())
        {
            return EXIT_FAILURE;
        }

        auto& Input = App.GetInput();
        SDL_Window* const Window = App.GetWindow();

        const auto& Renderer = App.GetRenderer();
        const auto& DeviceModule = App.GetDevice();
        const auto& SwapChainModule = App.GetSwapChain();
        const auto& MemoryModule = App.GetMemory();
        const auto& MeshRegistryModule = App.GetMeshRegistry();

        volkInitialize();
        volkLoadInstance(Renderer->GetInstance());
        volkLoadDevice(DeviceModule->GetLogicalDevice());

        const luvk::ScopeCaller<[]
        {
            volkFinalize();
        }> FreeVolk{};

        Cube CubeMesh{MeshRegistryModule, DeviceModule, SwapChainModule, MemoryModule};
        Triangle TriangleMesh{MeshRegistryModule, DeviceModule, SwapChainModule, MemoryModule};
        Pixel PixelMesh{MeshRegistryModule, DeviceModule, SwapChainModule};

        Renderer->InitializeRenderLoop();

        luvk::InitializeGlslang();

        bool CanRender = true;
        Camera AppCamera{};

        ImGuiLayer GuiLayer;
        if (!GuiLayer.Initialize(Window,
                                 MeshRegistryModule,
                                 DeviceModule,
                                 SwapChainModule,
                                 MemoryModule))
        {
            return EXIT_FAILURE;
        }

        Input.BindEvent(SDL_WINDOWEVENT,
                        [&](const SDL_Event& Event)
                        {
                            if (Event.window.event == SDL_WINDOWEVENT_RESIZED ||
                                Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                                Event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                            {
                                Renderer->SetPaused(true);
                                {
                                    std::int32_t NewW = 0;
                                    std::int32_t NewH = 0;
                                    SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                                    Renderer->Refresh({static_cast<std::uint32_t>(NewW), static_cast<std::uint32_t>(NewH)});
                                }
                                Renderer->SetPaused(false);
                            }
                            else if (Event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                            {
                                Renderer->SetPaused(true);
                                CanRender = false;
                            }
                            else if (Event.window.event == SDL_WINDOWEVENT_RESTORED)
                            {
                                Renderer->SetPaused(false);
                                CanRender = true;
                            }
                        });

        Input.BindEvent(SDL_MOUSEBUTTONDOWN,
                        [&](const SDL_Event& Event)
                        {
                            if (Event.button.button == SDL_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse)
                            {
                                std::int32_t NewW = 0;
                                std::int32_t NewH = 0;
                                SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                                const glm::vec2 Position{2.F * static_cast<float>(Event.button.x) / static_cast<float>(NewW) - 1.F,
                                                         2.F * static_cast<float>(Event.button.y) / static_cast<float>(NewH) - 1.F};

                                TriangleMesh.AddInstance(Position);
                            }
                        });

        Input.BindEvent(SDL_MOUSEMOTION,
                        [&](const SDL_Event& Event)
                        {
                            if (Input.LeftHeld() && !ImGui::GetIO().WantCaptureMouse)
                            {
                                std::int32_t NewW = 0;
                                std::int32_t NewH = 0;
                                SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                                const glm::vec2 Position{2.F * static_cast<float>(Event.motion.x) / static_cast<float>(NewW) - 1.F,
                                                         2.F * static_cast<float>(Event.motion.y) / static_cast<float>(NewH) - 1.F};

                                PixelMesh.AddInstance(Position);
                            }
                        });

        Input.BindEvent(SDL_USEREVENT,
                        [&](const SDL_Event& Event)
                        {
                            [[maybe_unused]] const auto _ = GuiLayer.ProcessEvent(Event);
                        });

        auto LastTime = std::chrono::high_resolution_clock::now();
        static auto RenderLoop = [&]
        {
            Input.ProcessEvents();
            if (!Input.Running())
            {
                return false;
            }

            std::int32_t CurrentWidth = 0;
            std::int32_t CurrentHeight = 0;
            SDL_Vulkan_GetDrawableSize(Window, &CurrentWidth, &CurrentHeight);

            if (!CanRender)
            {
                return true;
            }

            const auto CurrentTime = std::chrono::high_resolution_clock::now();
            const float DeltaTime = std::chrono::duration<float>(CurrentTime - LastTime).count();
            LastTime = CurrentTime;

            AppCamera.Update(DeltaTime, Input);

            glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(CurrentWidth) / static_cast<float>(CurrentHeight), 0.1F, 10.F);
            Proj[1][1] *= -1.F;
            CubeMesh.Update(DeltaTime, AppCamera.GetViewMatrix(), Proj);
            TriangleMesh.Update(DeltaTime);

            if (GuiLayer.IsInitialized())
            {
                GuiLayer.NewFrame(DeltaTime);

                Renderer->EnqueueCommand([&GuiLayer](const VkCommandBuffer& Cmd)
                {
                    GuiLayer.Render(Cmd);
                });
            }

            Renderer->DrawFrame();
            return true;
        };

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        while (RenderLoop());

        DeviceModule->WaitIdle();
        GuiLayer.Shutdown();
        luvk::FinalizeGlslang();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception)
    {
        std::printf("Error: %s\n", Exception.what());
    }

    return EXIT_FAILURE;
}
