// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <string>
#include <volk.h>
#include <luvk/Types/Vector.hpp>

namespace luvk
{
    class Device;
    class DescriptorPool;
    class Memory;
    class Texture;
    class Pipeline;
    class DescriptorSet;
}

namespace luvk_example
{
    class ShaderImage
    {
        float                                m_TotalTime{0.0f};
        VkRenderPass                         m_RenderPass{VK_NULL_HANDLE};
        VkFramebuffer                        m_Framebuffer{VK_NULL_HANDLE};
        std::shared_ptr<luvk::Texture>       m_Texture{};
        std::shared_ptr<luvk::Pipeline>      m_Pipeline{};
        std::shared_ptr<luvk::DescriptorSet> m_DescriptorSet{};

        std::shared_ptr<luvk::Device>         m_Device;
        std::shared_ptr<luvk::DescriptorPool> m_Pool;
        std::shared_ptr<luvk::Memory>         m_Memory;

        luvk::Vector<std::uint32_t> m_CachedVertexShader{};

    public:
        ShaderImage(std::shared_ptr<luvk::Device>         Device,
                    std::shared_ptr<luvk::DescriptorPool> Pool,
                    std::shared_ptr<luvk::Memory>         Memory);
        ~ShaderImage();

        void Update(const VkCommandBuffer& Cmd, float DeltaTime);

        [[nodiscard]] bool Compile(const std::string& Source, std::string& OutError);
        void               Reset();

        [[nodiscard]] VkDescriptorSet GetDescriptorSet() const;

    private:
        void Initialize();
        void CreatePipeline(const luvk::Vector<std::uint32_t>& FragSpirv);
        void TransitionLayout() const;
    };
}
