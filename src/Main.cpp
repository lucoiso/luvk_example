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

#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Buffer.hpp>
#include <luvk/Libraries/ShaderCompiler.hpp>

using namespace luvk_example;

int main()
{
    constexpr std::uint32_t WindowWidth = 800U;
    constexpr std::uint32_t WindowHeight = 600U;

    Application App(WindowWidth, WindowHeight);
    if (!App.Initialize())
    {
        return EXIT_FAILURE;
    }

    auto& Input = App.GetInput();
    SDL_Window* const Window = Input.GetWindow();
    auto Renderer = App.GetRenderer();
    auto DeviceModule = App.GetDevice();
    auto SwapChainModule = App.GetSwapChain();
    auto CommandPoolModule = App.GetCommandPool();
    auto MemoryModule = App.GetMemory();
    auto MeshRegistryModule = App.GetMeshRegistry();
    auto ThreadPoolModule = App.GetThreadPool();

    auto CubePipeline = std::make_shared<luvk::Pipeline>();
    auto TriPipeline = std::make_shared<luvk::Pipeline>();

    constexpr auto CubeVertSrc = R"(#version 450
                                    layout(push_constant) uniform Push {
                                        mat4 mvp;
                                        vec4 color;
                                    } pc;
                                    layout(location = 0) in vec3 inPos;
                                    layout(location = 0) out vec4 vColor;
                                    void main() {
                                        gl_Position = pc.mvp * vec4(inPos, 1.0);
                                        vColor = pc.color;
                                    })";

    constexpr auto CubeFragSrc = R"(#version 450
                                    layout(location = 0) in vec4 vColor;
                                    layout(location = 0) out vec4 outColor;
                                    void main() {
                                        outColor = vColor;
                                    })";

    constexpr auto TriVertSrc = R"(#version 450
                                   layout(location = 0) in vec2 inPos;
                                   layout(location = 1) in vec2 offset;
                                   layout(location = 2) in float angle;
                                   layout(location = 3) in vec4 instColor;
                                   layout(location = 0) out vec4 vColor;
                                   void main() {
                                       mat2 R = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
                                       vec2 pos = R * inPos + offset;
                                       gl_Position = vec4(pos, 0.0, 1.0);
                                       vColor = instColor;
                                   })";

    constexpr auto TriFragSrc = R"(#version 450
                                   layout(location = 0) in vec4 vColor;
                                   layout(location = 0) out vec4 outColor;
                                   void main() {
                                       outColor = vColor;
                                   })";

    auto CubeVert = luvk::CompileGLSLToSPIRV(CubeVertSrc, EShLangVertex);
    auto CubeFrag = luvk::CompileGLSLToSPIRV(CubeFragSrc, EShLangFragment);
    auto TriVert = luvk::CompileGLSLToSPIRV(TriVertSrc, EShLangVertex);
    auto TriFrag = luvk::CompileGLSLToSPIRV(TriFragSrc, EShLangFragment);

    const VkExtent2D Extent = SwapChainModule->GetExtent();
    const std::array Formats{SwapChainModule->m_Arguments.Format};

    constexpr VkVertexInputBindingDescription CubeBinding{0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX};
    constexpr VkVertexInputAttributeDescription CubeAttr{0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0};
    constexpr VkPushConstantRange CubePC{VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::mat4) + sizeof(glm::vec4)};

    CubePipeline->CreateGraphicsPipeline(DeviceModule,
                                         {.Extent = Extent,
                                          .ColorFormats = Formats,
                                          .RenderPass = SwapChainModule->GetRenderPass(),
                                          .Subpass = 0,
                                          .VertexShader = CubeVert,
                                          .FragmentShader = CubeFrag,
                                          .SetLayouts = {},
                                          .Bindings = std::array{CubeBinding},
                                          .Attributes = std::array{CubeAttr},
                                          .PushConstants = std::array{CubePC},
                                          .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST});

    constexpr std::array TriBindings{VkVertexInputBindingDescription{0, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX},
                                     VkVertexInputBindingDescription{1, sizeof(luvk::MeshInstance), VK_VERTEX_INPUT_RATE_INSTANCE}};

    const std::array TriAttrs{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
                              VkVertexInputAttributeDescription{1, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(luvk::MeshInstance, Offset)},
                              VkVertexInputAttributeDescription{2, 1, VK_FORMAT_R32_SFLOAT, offsetof(luvk::MeshInstance, Angle)},
                              VkVertexInputAttributeDescription{3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(luvk::MeshInstance, Color)}};

    TriPipeline->CreateGraphicsPipeline(DeviceModule,
                                        {.Extent = Extent,
                                         .ColorFormats = Formats,
                                         .RenderPass = SwapChainModule->GetRenderPass(),
                                         .Subpass = 0,
                                         .VertexShader = TriVert,
                                         .FragmentShader = TriFrag,
                                         .SetLayouts = {},
                                         .Bindings = std::array{TriBindings.at(0), TriBindings.at(1)},
                                         .Attributes = std::array{TriAttrs.at(0), TriAttrs.at(1), TriAttrs.at(2), TriAttrs.at(3)},
                                         .Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                         .CullMode = VK_CULL_MODE_NONE});

    constexpr std::array<glm::vec3, 8> CubeVertices{{{-0.5f, -0.5f, -0.5f},
                                                     { 0.5f, -0.5f, -0.5f},
                                                     { 0.5f,  0.5f, -0.5f},
                                                     {-0.5f,  0.5f, -0.5f},
                                                     {-0.5f, -0.5f,  0.5f},
                                                     { 0.5f, -0.5f,  0.5f},
                                                     { 0.5f,  0.5f,  0.5f},
                                                     {-0.5f,  0.5f,  0.5f}}};
    constexpr std::array<std::uint16_t, 36> CubeIndices{{0, 1, 2, 2, 3, 0,
                                                         4, 5, 6, 6, 7, 4,
                                                         0, 1, 5, 5, 4, 0,
                                                         2, 3, 7, 7, 6, 2,
                                                         1, 2, 6, 6, 5, 1,
                                                         3, 0, 4, 4, 7, 3}};

    constexpr std::array<glm::vec2, 3> TriVertices{{{-0.05f, -0.05f},
                                                    { 0.05f, -0.05f},
                                                    { 0.0f,   0.1f}}};
    constexpr std::array<std::uint16_t, 3> TriIndices{{0, 1, 2}};

    constexpr std::array<glm::vec2, 12> PixelVertices{{{ 0.01f,  0.0f},
                                                       { 0.00866f,  0.005f},
                                                       { 0.005f,  0.00866f},
                                                       { 0.0f,   0.01f},
                                                       {-0.005f, 0.00866f},
                                                       {-0.00866f, 0.005f},
                                                       {-0.01f,  0.0f},
                                                       {-0.00866f, -0.005f},
                                                       {-0.005f, -0.00866f},
                                                       { 0.0f,  -0.01f},
                                                       { 0.005f, -0.00866f},
                                                       { 0.00866f, -0.005f}}};
    constexpr std::array<std::uint16_t, 30> PixelIndices{{0, 1, 2,
                                                          0, 2, 3,
                                                          0, 3, 4,
                                                          0, 4, 5,
                                                          0, 5, 6,
                                                          0, 6, 7,
                                                          0, 7, 8,
                                                          0, 8, 9,
                                                          0, 9, 10,
                                                          0, 10, 11}};

    auto CubeUBO = std::make_shared<luvk::Buffer>();
    CubeUBO->CreateBuffer(MemoryModule,
                          {.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, .Size = sizeof(glm::mat4) + sizeof(glm::vec4), .MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU});

    const std::size_t CubeMeshIndex = MeshRegistryModule->RegisterMesh(std::as_bytes(std::span{CubeVertices}),
                                                                       std::as_bytes(std::span{CubeIndices}),
                                                                       VK_NULL_HANDLE,
                                                                       nullptr,
                                                                       nullptr,
                                                                       nullptr,
                                                                       CubeUBO,
                                                                       {},
                                                                       CubePipeline,
                                                                       DeviceModule);

    const std::size_t TriMeshIndex = MeshRegistryModule->RegisterMesh(std::as_bytes(std::span{TriVertices}),
                                                                      std::as_bytes(std::span{TriIndices}),
                                                                      VK_NULL_HANDLE,
                                                                      nullptr,
                                                                      nullptr,
                                                                      nullptr,
                                                                      nullptr,
                                                                      {},
                                                                      TriPipeline,
                                                                      DeviceModule);

    const std::size_t PixelMeshIndex = MeshRegistryModule->RegisterMesh(std::as_bytes(std::span{PixelVertices}),
                                                                        std::as_bytes(std::span{PixelIndices}),
                                                                        VK_NULL_HANDLE,
                                                                        nullptr,
                                                                        nullptr,
                                                                        nullptr,
                                                                        nullptr,
                                                                        {},
                                                                        TriPipeline,
                                                                        DeviceModule);

    Cube CubeMesh{MeshRegistryModule, CubeMeshIndex};
    Triangle TriangleMesh{MeshRegistryModule, TriMeshIndex};
    Pixel PixelMesh{MeshRegistryModule, PixelMeshIndex};

    Renderer->InitializeRenderLoop(DeviceModule, SwapChainModule, CommandPoolModule, MeshRegistryModule, ThreadPoolModule);
    Renderer->GetEventSystem().AddNode(luvk::EventNode::NewNode([]
                                       {
                                           SDL_Log("Render loop initialized");
                                       }),
                                       luvk::RendererEvents::OnRenderLoopInitialized);

    luvk::InitializeGlslang();

    bool CanRender = true;
    Camera AppCamera{};

    Input.BindEvent(SDL_WINDOWEVENT,
                    [&](SDL_Event const& e)
                    {
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                            e.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                        {
                            Renderer->SetPaused(true);
                            int NewW = 0, NewH = 0;
                            SDL_Vulkan_GetDrawableSize(Window, &NewW, &NewH);
                            Renderer->Refresh({static_cast<std::uint32_t>(NewW), static_cast<std::uint32_t>(NewH)});
                            Renderer->SetPaused(false);
                        }
                        else if (e.window.event == SDL_WINDOWEVENT_MINIMIZED)
                        {
                            Renderer->SetPaused(true);
                            CanRender = false;
                        }
                        else if (e.window.event == SDL_WINDOWEVENT_RESTORED)
                        {
                            Renderer->SetPaused(false);
                            CanRender = true;
                        }
                    });

    Input.BindEvent(SDL_MOUSEBUTTONDOWN,
                    [&](SDL_Event const& e)
                    {
                        if (e.button.button == SDL_BUTTON_RIGHT)
                        {
                            int W = 0, H = 0;
                            SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                            const glm::vec2 Position{2.F * e.button.x / static_cast<float>(W) - 1.F, 2.F * e.button.y / static_cast<float>(H) - 1.F};
                            TriangleMesh.AddInstance(Position);
                        }
                    });

    Input.BindEvent(SDL_MOUSEMOTION,
                    [&](SDL_Event const& e)
                    {
                        if (Input.LeftHeld())
                        {
                            int W = 0, H = 0;
                            SDL_Vulkan_GetDrawableSize(Window, &W, &H);
                            const glm::vec2 Position{2.F * e.motion.x / static_cast<float>(W) - 1.F, 2.F * e.motion.y / static_cast<float>(H) - 1.F};
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
