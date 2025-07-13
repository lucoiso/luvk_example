// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <vulkan/vulkan.h>

struct SDL_Window;

namespace luvk
{
    class Renderer;
}

namespace luvk_example
{
    class Application;

    class ImGuiLayer
    {
        Application* m_App{};
        VkDescriptorPool m_Pool{VK_NULL_HANDLE};
        bool m_Initialized{false};

    public:
        ImGuiLayer() = default;
        ~ImGuiLayer() = default;

        bool Init(Application& App);
        void Render();
        void Shutdown();
    };
} // namespace luvk_example

