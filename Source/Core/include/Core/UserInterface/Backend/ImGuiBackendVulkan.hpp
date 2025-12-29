/*
* Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <imgui.h>
#include <memory>
#include <volk.h>

namespace luvk
{
    class Renderer;
    class SwapChain;
    class CommandPool;
    class Synchronization;
    class Draw;
}

namespace Core
{
    class ImGuiMesh;

    struct CORE_API ViewportData
    {
        std::int32_t                    ImageIndex{-1};
        VkSurfaceKHR                    Surface{VK_NULL_HANDLE};
        std::shared_ptr<ImGuiMesh>      Mesh{nullptr};
        std::unique_ptr<luvk::Renderer> Renderer{nullptr};
        luvk::EventHandle               SurfaceDeleteHandle;

        ~ViewportData();
    };

    class CORE_API ImGuiBackendVulkan
    {
        luvk::Renderer*            m_Renderer;
        std::shared_ptr<ImGuiMesh> m_Mesh{};

    public:
        ImGuiBackendVulkan() = delete;
        explicit ImGuiBackendVulkan(luvk::Renderer* Renderer);
        ~ImGuiBackendVulkan();

        void NewFrame() const;
        void Render(VkCommandBuffer Cmd) const;

        [[nodiscard]] luvk::Renderer* GetRenderer() const
        {
            return m_Renderer;
        }

        [[nodiscard]] std::shared_ptr<ImGuiMesh> GetMesh() const noexcept
        {
            return m_Mesh;
        }

    private:
        static void CreateWindow(ImGuiViewport* Viewport);
        static void DestroyWindow(ImGuiViewport* Viewport);
        static void SetWindowSize(ImGuiViewport* Viewport, ImVec2 Size);
        static void RenderWindow(ImGuiViewport* Viewport, void* RenderArg);
        static void SwapBuffers(ImGuiViewport* Viewport, void* RenderArg);
    };
}
