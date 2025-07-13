// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Application.hpp"

#include <SDL2/SDL_vulkan.h>

#ifdef main
#undef main
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>
#include <array>

#include "luvk_example/Camera.hpp"
#include "luvk_example/Cube.hpp"
#include "luvk_example/Pixel.hpp"
#include "luvk_example/Triangle.hpp"
#include "luvk_example/ImGuiLayer.hpp"

#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

int main()
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
        auto Renderer = App.GetRenderer();
        auto DeviceModule = App.GetDevice();
        auto SwapChainModule = App.GetSwapChain();
        auto CommandPoolModule = App.GetCommandPool();
        auto MemoryModule = App.GetMemory();
        auto MeshRegistryModule = App.GetMeshRegistry();
        auto ThreadPoolModule = App.GetThreadPool();

        Cube CubeMesh{MeshRegistryModule, DeviceModule, SwapChainModule, MemoryModule};
        Triangle TriangleMesh{MeshRegistryModule, DeviceModule, SwapChainModule, MemoryModule};
        Pixel PixelMesh{MeshRegistryModule, DeviceModule, SwapChainModule};

        Renderer->GetEventSystem().AddNode(luvk::EventNode::NewNode([]
                                           {
                                               SDL_Log("Render loop initialized");
                                           }),
                                           luvk::RendererEvents::OnRenderLoopInitialized);

        Renderer->InitializeRenderLoop();

        luvk::InitializeGlslang();

        bool CanRender = true;
        Camera AppCamera{};

        Input.BindEvent(SDL_WINDOWEVENT,
                        [&](SDL_Event const& Event)
                        {
                            if (Event.window.event == SDL_WINDOWEVENT_RESIZED ||
                                Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                                Event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                            {
                                Renderer->SetPaused(true);
                                int NewW = 0, NewH = 0;
                                SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                                Renderer->Refresh({static_cast<std::uint32_t>(NewW), static_cast<std::uint32_t>(NewH)});
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
                        [&](SDL_Event const& Event)
                        {
                            if (Event.button.button == SDL_BUTTON_RIGHT)
                            {
                                int W = 0, H = 0;
                                SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                                const glm::vec2 Position{2.F * static_cast<float>(Event.button.x) / static_cast<float>(W) - 1.F,
                                                         2.F * static_cast<float>(Event.button.y) / static_cast<float>(H) - 1.F};
                                TriangleMesh.AddInstance(Position);
                            }
                        });

        Input.BindEvent(SDL_MOUSEMOTION,
                        [&](SDL_Event const& Event)
                        {
                            if (Input.LeftHeld())
                            {
                                int W = 0, H = 0;
                                SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                                const glm::vec2 Position{2.F * static_cast<float>(Event.motion.x) / static_cast<float>(W) - 1.F,
                                                         2.F * static_cast<float>(Event.motion.y) / static_cast<float>(H) - 1.F};
                                PixelMesh.AddInstance(Position);
                            }
                        });

        auto LastTime = std::chrono::high_resolution_clock::now();
        int Frames = 0;
        auto FpsTime = LastTime;

        while (Input.Running())
        {
            Input.ProcessEvents();
            if (!Input.Running())
            {
                break;
            }

            int CurrentWidth = 0;
            int CurrentHeight = 0;
            SDL_Vulkan_GetDrawableSize(Window, &CurrentWidth, &CurrentHeight);

            if (!CanRender)
            {
                continue;
            }

            auto CurrentTime = std::chrono::high_resolution_clock::now();
            float DeltaTime = std::chrono::duration<float>(CurrentTime - LastTime).count();
            LastTime = CurrentTime;

            AppCamera.Update(DeltaTime, Input);

            glm::mat4 Proj = glm::perspective(glm::radians(45.F), static_cast<float>(CurrentWidth) / static_cast<float>(CurrentHeight), 0.1F, 10.F);
            Proj[1][1] *= -1.F;
            CubeMesh.Update(DeltaTime, AppCamera.GetViewMatrix(), Proj);
            TriangleMesh.Update(DeltaTime);

            Renderer->DrawFrame();

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

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception)
    {
        std::printf("Error: %s\n", Exception.what());
    }

    return EXIT_FAILURE;
}
