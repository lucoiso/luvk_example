/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <string>
#include <volk.h>
#include "Core/UserInterface/ImGuiLayerBase.hpp"
#include "UserInterface/Components/ShaderImage.hpp"

struct ImGuiInputTextCallbackData;

namespace UserInterface
{
    class ImGuiLayer : public Core::ImGuiLayerBase
    {
        bool m_CompileRequested{false};
        bool        m_CompileSuccess{true};
        std::string m_ShaderCode;
        std::string m_StatusMessage{"Ready"};

        std::unique_ptr<ShaderImage> m_ShaderImage{};

    public:
        explicit ImGuiLayer(SDL_Window* Window, const std::shared_ptr<luvk::Renderer>& Renderer);

        ~ImGuiLayer() override;

        void Draw() override;
        void RecordCommands(VkCommandBuffer Cmd) const;
        void UpdatePreviewImmediate(float DeltaTime) const;

        void ExecutePendingCompile();

    private:
        void InitializeResources(const std::shared_ptr<luvk::Renderer>& Renderer);

        void DrawEditor();
        void DrawTexture() const;

        static int InputTextCallback(ImGuiInputTextCallbackData* Data);
    };
}
