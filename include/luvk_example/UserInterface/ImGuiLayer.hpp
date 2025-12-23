// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <string>
#include <volk.h>
#include "luvk_example/UserInterface/ImGuiLayerBase.hpp"
#include "luvk_example/UserInterface/Components/ShaderImage.hpp"

struct ImGuiInputTextCallbackData;

namespace luvk
{
    class DescriptorPool;
}

namespace luvk_example
{
    class ImGuiLayer : public ImGuiLayerBase
    {
        bool        m_CompileSuccess{true};
        std::string m_ShaderCode;
        std::string m_StatusMessage{"Ready"};

        std::unique_ptr<ShaderImage> m_ShaderImage{};

    public:
        explicit ImGuiLayer(SDL_Window*                                  Window,
                            VkInstance                                   Instance,
                            std::shared_ptr<luvk::Device> const&         Device,
                            std::shared_ptr<luvk::DescriptorPool> const& Pool,
                            std::shared_ptr<luvk::SwapChain> const&      Swap,
                            std::shared_ptr<luvk::Memory> const&         Memory);

        ~ImGuiLayer() override;

        void Draw() override;
        void UpdatePreview(const VkCommandBuffer Cmd);

    private:
        void InitializeResources(std::shared_ptr<luvk::Device> const&         Device,
                                 std::shared_ptr<luvk::DescriptorPool> const& Pool,
                                 std::shared_ptr<luvk::Memory> const&         Memory);

        void DrawDockspace() const;
        void DrawEditor();
        void DrawTexture() const;

        void CompileShader();

        static int InputTextCallback(ImGuiInputTextCallbackData* Data);
    };
}
