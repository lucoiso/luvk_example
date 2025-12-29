/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#include "Core/UserInterface/Backend/ImGuiBackendVulkan.hpp"
#include <luvk/Interfaces/IServiceLocator.hpp>
#include <luvk/Modules/CommandPool.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Draw.hpp>
#include <luvk/Modules/Instance.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/Renderer.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Modules/Synchronization.hpp>
#include "Core/Meshes/ImGuiMesh.hpp"

#undef CreateWindow

using namespace Core;

static VkQueue GetGraphicsQueue(const luvk::Device* Device)
{
    return Device->GetQueue(VK_QUEUE_GRAPHICS_BIT);
}

ViewportData::~ViewportData() = default;

ImGuiBackendVulkan::ImGuiBackendVulkan(luvk::Renderer* Renderer)
    : m_Renderer(Renderer)
{
    ImGuiIO& IO                = ImGui::GetIO();
    IO.BackendRendererName     = "ImGuiBackendVulkan_LUVK";
    IO.BackendRendererUserData = static_cast<void*>(this);
    IO.BackendFlags            |= ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports;

    m_Mesh = std::make_shared<ImGuiMesh>(Renderer->GetModule<luvk::Device>(), Renderer->GetModule<luvk::Memory>(), Renderer->GetModule<luvk::DescriptorPool>());

    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiPlatformIO& PlatformIO       = ImGui::GetPlatformIO();
        PlatformIO.Renderer_CreateWindow  = CreateWindow;
        PlatformIO.Renderer_DestroyWindow = DestroyWindow;
        PlatformIO.Renderer_SetWindowSize = SetWindowSize;
        PlatformIO.Renderer_RenderWindow  = RenderWindow;
        PlatformIO.Renderer_SwapBuffers   = SwapBuffers;
    }
}

ImGuiBackendVulkan::~ImGuiBackendVulkan()
{
    ImGuiIO& IO                = ImGui::GetIO();
    IO.BackendRendererUserData = nullptr;
    IO.BackendRendererName     = nullptr;
}

void ImGuiBackendVulkan::NewFrame() const
{
    ImGui::NewFrame();
}

void ImGuiBackendVulkan::Render(const VkCommandBuffer Cmd) const
{
    ImGui::Render();
    const ImDrawData* const DrawData = ImGui::GetDrawData();
    m_Mesh->UpdateBuffers(DrawData);
    m_Mesh->Render(Cmd);
}

void ImGuiBackendVulkan::CreateWindow(ImGuiViewport* const Viewport)
{
    const auto Backend  = static_cast<ImGuiBackendVulkan*>(ImGui::GetIO().BackendRendererUserData);
    const auto Renderer = Backend->GetRenderer();

    auto* const Data           = new ViewportData();
    Data->Renderer             = std::make_unique<luvk::Renderer>(Renderer);
    Viewport->RendererUserData = static_cast<void*>(Data);

    ImGui::GetPlatformIO().Platform_CreateVkSurface(Viewport,
                                                    reinterpret_cast<ImU64>(Renderer->GetModule<luvk::Instance>()->GetHandle()),
                                                    nullptr,
                                                    reinterpret_cast<ImU64*>(&Data->Surface));

    auto* SwapModule        = Data->Renderer->RegisterModule<luvk::SwapChain>();
    auto  DeleteSurfaceNode = luvk::EventNode::NewNode([Data]
                                                       {
                                                           if (const auto* InstanceMod = Data->Renderer->GetModule<luvk::Instance>())
                                                           {
                                                               vkDestroySurfaceKHR(InstanceMod->GetHandle(), Data->Surface, nullptr);
                                                               Data->Surface = VK_NULL_HANDLE;
                                                           }
                                                       },
                                                       true);

    Data->SurfaceDeleteHandle = SwapModule->GetEventSystem().AddNode(std::move(DeleteSurfaceNode), luvk::ModuleEvents::OnShutdown);

    Data->Renderer->RegisterModule<luvk::CommandPool>();
    Data->Renderer->RegisterModule<luvk::Synchronization>();
    auto* DrawModule = Data->Renderer->RegisterModule<luvk::Draw>();
    Data->Renderer->InitializeModules();

    SwapModule->CreateSwapChain({.Extent = {static_cast<uint32_t>(Viewport->Size.x), static_cast<uint32_t>(Viewport->Size.y)}, .Surface = Data->Surface});

    Data->Mesh = std::make_shared<ImGuiMesh>(Renderer->GetModule<luvk::Device>(), Renderer->GetModule<luvk::Memory>(), Backend->GetMesh()->GetMaterial());

    DrawModule->AddDrawCallback([Data, Viewport](const VkCommandBuffer Cmd, [[maybe_unused]] const std::uint32_t Frame)
    {
        Data->Mesh->UpdateBuffers(Viewport->DrawData);
        Data->Mesh->Render(Cmd);
    });
}

void ImGuiBackendVulkan::DestroyWindow(ImGuiViewport* const Viewport)
{
    const auto Data = static_cast<ViewportData*>(Viewport->RendererUserData);
    if (!Data)
    {
        return;
    }

    const auto* Device = Data->Renderer->GetModule<luvk::Device>();
    Device->WaitQueue(VK_QUEUE_GRAPHICS_BIT);

    Data->Renderer->Shutdown();

    delete Data;
    Viewport->RendererUserData = nullptr;
}

void ImGuiBackendVulkan::SetWindowSize(ImGuiViewport* const Viewport, const ImVec2 Size)
{
    const auto Data = static_cast<ViewportData*>(Viewport->RendererUserData);
    if (!Data)
    {
        return;
    }

    const auto* Device = Data->Renderer->GetModule<luvk::Device>();
    Device->WaitQueue(VK_QUEUE_GRAPHICS_BIT);

    auto* SwapModule = Data->Renderer->RegisterModule<luvk::SwapChain>();
    SwapModule->Recreate({static_cast<uint32_t>(Size.x), static_cast<uint32_t>(Size.y)});
}

void ImGuiBackendVulkan::RenderWindow(ImGuiViewport* const Viewport, void*)
{
    const auto Data = static_cast<ViewportData*>(Viewport->RendererUserData);
    if (!Data)
    {
        return;
    }

    const auto* DrawModule = Data->Renderer->RegisterModule<luvk::Draw>();
    DrawModule->RenderFrame();
}

void ImGuiBackendVulkan::SwapBuffers([[maybe_unused]] ImGuiViewport* const Viewport, void*) {}
