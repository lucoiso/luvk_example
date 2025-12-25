// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "UserInterface/Application/Application.hpp"
#include <imgui.h>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Draw.hpp>
#include <SDL3/SDL_events.h>
#include "luvk/Libraries/ShaderCompiler.hpp"
#include "UserInterface/Components/Camera.hpp"
#include "UserInterface/Layers/ImGuiLayer.hpp"
#include "UserInterface/Meshes/Cube.hpp"
#include "UserInterface/Meshes/Pixel.hpp"
#include "UserInterface/Meshes/Triangle.hpp"

using namespace UserInterface;

std::shared_ptr<Application> Application::s_Instance = nullptr;

Application::Application(const std::uint32_t Width, const std::uint32_t Height)
    : ApplicationBase(Width, Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE),
      m_Camera(std::make_shared<Camera>(m_Input)) {}

bool Application::Initialize()
{
    luvk::InitializeShaderCompiler();

    if (ApplicationBase::Initialize())
    {
        CreateScene();
        return true;
    }

    return false;
}

void Application::Shutdown()
{
    const luvk::RenderModules& Modules = m_Renderer->GetModules();
    Modules.DeviceModule->WaitIdle();

    m_CubeMesh.reset();
    m_TriangleMesh.reset();
    m_PixelMesh.reset();
    m_Camera.reset();

    luvk::ShutdownShaderCompiler();
    ApplicationBase::Shutdown();
    s_Instance.reset();
}

std::shared_ptr<Application> Application::GetInstance()
{
    if (!s_Instance)
    {
        s_Instance = std::shared_ptr<Application>(new Application(800, 600),
                                                  [](const Application* Instance)
                                                  {
                                                      delete Instance;
                                                      volkFinalize();
                                                  });
    }

    return s_Instance;
}

void Application::CreateScene()
{
    const luvk::RenderModules& Modules = m_Renderer->GetModules();

    m_CubeMesh     = std::make_unique<Cube>(Modules.DeviceModule, Modules.SwapChainModule, Modules.MemoryModule, Modules.DescriptorPoolModule);
    m_TriangleMesh = std::make_unique<Triangle>(Modules.DeviceModule, Modules.SwapChainModule, Modules.MemoryModule, Modules.DescriptorPoolModule);
    m_PixelMesh    = std::make_unique<Pixel>(Modules.DeviceModule, Modules.SwapChainModule, Modules.MemoryModule);
}

bool Application::PreRenderCallback(VkCommandBuffer CommandBuffer)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        m_Camera->Update(m_DeltaTime);
    }

    m_CubeMesh->Tick(m_DeltaTime);
    m_TriangleMesh->Tick(m_DeltaTime);
    m_PixelMesh->Tick(m_DeltaTime);

    m_TriangleMesh->Compute(CommandBuffer);

    dynamic_cast<ImGuiLayer*>(m_ImGuiLayer.get())->UpdatePreview(CommandBuffer);

    return ApplicationBase::PreRenderCallback(CommandBuffer);
}

bool Application::DrawCallback(const VkCommandBuffer CommandBuffer, const std::uint32_t CurrentFrame)
{
    m_CubeMesh->Render(CommandBuffer, CurrentFrame);
    m_TriangleMesh->Render(CommandBuffer, CurrentFrame);
    m_PixelMesh->Render(CommandBuffer, CurrentFrame);

    return ApplicationBase::DrawCallback(CommandBuffer, CurrentFrame);
}

void Application::UserEventCallback(const SDL_Event& Event)
{
    ApplicationBase::UserEventCallback(Event);

    if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && Event.button.button == SDL_BUTTON_RIGHT && !ImGui::GetIO().WantCaptureMouse)
    {
        std::int32_t NewW = 0;
        std::int32_t NewH = 0;
        SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);

        const glm::vec2 Position{2.F * Event.button.x / static_cast<float>(NewW) - 1.F,
                                 2.F * Event.button.y / static_cast<float>(NewH) - 1.F};

        m_TriangleMesh->AddInstance(Position);
    }

    if (Event.type == SDL_EVENT_MOUSE_MOTION && m_Input->LeftHeld() && !ImGui::GetIO().WantCaptureMouse)
    {
        std::int32_t NewW = 0;
        std::int32_t NewH = 0;
        SDL_GetWindowSizeInPixels(m_Window, &NewW, &NewH);

        const glm::vec2 Position{2.F * Event.motion.x / static_cast<float>(NewW) - 1.F,
                                 2.F * Event.motion.y / static_cast<float>(NewH) - 1.F};

        m_PixelMesh->AddInstance(Position);
    }
}

void Application::SetupDeviceExtensions() const
{
    ApplicationBase::SetupDeviceExtensions();
    const luvk::RenderModules& Modules = m_Renderer->GetModules();

    if (auto& DevExt = Modules.DeviceModule->GetExtensions();
        DevExt.HasAvailableExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME))
    {
        DevExt.SetExtensionState("", VK_EXT_MESH_SHADER_EXTENSION_NAME, true);
    }
}

void* Application::GetDeviceFeatureChain() const
{
    static VkPhysicalDeviceMeshShaderFeaturesEXT MeshShaderFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
                                                                    .pNext = ApplicationBase::GetDeviceFeatureChain(),
                                                                    .taskShader = VK_TRUE,
                                                                    .meshShader = VK_TRUE};

    return &MeshShaderFeatures;
}
