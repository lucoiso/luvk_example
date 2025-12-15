// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <string>
#include <luvk/Types/Vector.hpp>
#include <volk.h>
#include "luvk_example/Base/ImGui/ImGuiLayerBase.hpp"

namespace luvk
{
    class Image;
    class Sampler;
    class Pipeline;
    class DescriptorSet;
} // namespace luvk

namespace luvk_example
{
    class ImGuiLayer : public ImGuiLayerBase
    {
        bool        m_CompileSuccess{true};
        std::string m_ShaderCode;
        std::string m_StatusMessage{"Ready"};
        float       m_TotalTime{0.0f};

        std::shared_ptr<luvk::Image>         m_PreviewImage{};
        std::shared_ptr<luvk::Sampler>       m_PreviewSampler{};
        VkRenderPass                         m_PreviewRenderPass{VK_NULL_HANDLE};
        VkFramebuffer                        m_PreviewFramebuffer{VK_NULL_HANDLE};
        std::shared_ptr<luvk::Pipeline>      m_PreviewPipeline{};
        std::shared_ptr<luvk::DescriptorSet> m_TextureID{};
        luvk::Vector<std::uint32_t>          m_CachedDefaultShader{};
        luvk::Vector<std::uint32_t>          m_CachedVertexShader{};

    public:
        void InitializeEditorResources();
        void Draw() override;
        void UpdatePreview(const VkCommandBuffer& Cmd);

    private:
        void DrawDockspace() const;
        void DrawEditor();
        void DrawTexture() const;

        void PushStyle() const;
        void CompileShader();
        void CreatePreviewPipeline(const luvk::Vector<std::uint32_t>& FragSpirv);

        static int InputTextCallback(struct ImGuiInputTextCallbackData* Data);
        void       TransitionTextureToReadState() const;
    };
} // namespace luvk_example
