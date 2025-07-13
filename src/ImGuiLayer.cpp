// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/ImGuiLayer.hpp"
#include "luvk_example/Application.hpp"

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include <SDL2/SDL.h>

#include <luvk/Core/Renderer.hpp>
#include <luvk/Core/Device.hpp>

using namespace luvk_example;

bool ImGuiLayer::Init(Application& App)
{
    m_App = &App;
    auto renderer = App.GetRenderer();
    auto device = App.GetDevice();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    SDL_Window* window = App.GetWindow();
    ImGui_ImplSDL2_InitForVulkan(window);

    VkDescriptorPoolSize pool_sizes[] =
    {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000}
    };
    m_Pool = renderer->CreateExternalDescriptorPool(1000, pool_sizes);

    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = renderer->GetInstance();
    init_info.PhysicalDevice = renderer->GetPhysicalDevice();
    init_info.Device = renderer->GetLogicalDevice();
    init_info.QueueFamily = device->FindQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT).value();
    init_info.Queue = renderer->GetGraphicsQueue();
    init_info.DescriptorPool = m_Pool;
    init_info.RenderPass = renderer->GetRenderPass();
    init_info.MinImageCount = static_cast<uint32_t>(renderer->GetSwapChainImageCount());
    init_info.ImageCount = init_info.MinImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info);

    m_Initialized = true;
    return true;
}

void ImGuiLayer::Render()
{
    if (!m_Initialized)
    {
        return;
    }

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Info");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    static int count = 0;
    if (ImGui::Button("Test"))
    {
        ++count;
    }
    ImGui::SameLine();
    ImGui::Text("%d", count);
    ImGui::End();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    m_App->GetRenderer()->EnqueueCommand([draw_data](VkCommandBuffer cmd)
    {
        ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
    });

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiLayer::Shutdown()
{
    if (!m_Initialized)
    {
        return;
    }

    VkDevice device = m_App->GetRenderer()->GetLogicalDevice();
    if (device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    m_App->GetRenderer()->DestroyExternalDescriptorPool(m_Pool);
    m_Pool = VK_NULL_HANDLE;
    m_Initialized = false;
}

