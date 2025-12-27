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

namespace luvk
{
    class DescriptorPool;
}

namespace UserInterface
{
    class ImGuiLayer : public Core::ImGuiLayerBase
    {
        bool        m_CompileSuccess{true};
        std::string m_ShaderCode;
        std::string m_StatusMessage{"Ready"};

        std::unique_ptr<ShaderImage> m_ShaderImage{};

    public:
        explicit ImGuiLayer(SDL_Window*                            Window,
                            std::shared_ptr<luvk::Renderer> const& Renderer);

        ~ImGuiLayer() override;

        void Draw() override;
        void UpdatePreview(VkCommandBuffer Cmd) const;

    private:
        void InitializeResources(std::shared_ptr<luvk::Device> const&         Device,
                                 std::shared_ptr<luvk::DescriptorPool> const& Pool,
                                 std::shared_ptr<luvk::Memory> const&         Memory);

        void DrawEditor();
        void DrawTexture() const;

        void CompileShader();

        static int InputTextCallback(ImGuiInputTextCallbackData* Data);
    };
}