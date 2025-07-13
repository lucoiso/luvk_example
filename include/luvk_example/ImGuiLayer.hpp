// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "ImGuiBackend/ImGuiBackendSDL2.hpp"
#include "ImGuiBackend/ImGuiBackendLUVK.hpp"

#include <volk/volk.h>
#include <memory>
#include <SDL2/SDL.h>

namespace luvk_example
{
    class ImGuiLayer
    {
        ImGuiBackendSDL2 m_SdlBackend{};
        ImGuiBackendLUVK m_VkBackend{};

    public:
        ImGuiLayer() = default;

        bool Initialize(SDL_Window* Window, std::shared_ptr<luvk::Renderer> const& Renderer);
        void Shutdown();
        void NewFrame(float DeltaTime) const;
        void Render(VkCommandBuffer Cmd) const;
        bool ProcessEvent(SDL_Event const& Event) const;
    };
} // namespace luvk_example
