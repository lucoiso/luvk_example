/*
* Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <volk.h>

#include "luvk/Resources/DescriptorSet.hpp"

namespace luvk
{
    class Texture;
    class Pipeline;
    class DescriptorSet;
    class Device;
    class DescriptorPool;
    class Memory;
    class Renderer;
}

namespace UserInterface
{
    class ShaderImage
    {
        float m_TotalTime{0.0f};

        std::shared_ptr<luvk::Texture>       m_Texture{};
        std::shared_ptr<luvk::Pipeline>      m_Pipeline{};
        std::shared_ptr<luvk::DescriptorSet> m_DescriptorSet{};

        luvk::Device*                 m_Device{nullptr};
        luvk::DescriptorPool*         m_Pool{nullptr};
        luvk::Memory*                 m_Memory{nullptr};
        std::weak_ptr<luvk::Renderer> m_Renderer;

        std::vector<std::uint32_t> m_CachedVertexShader{};
        std::vector<std::uint32_t> m_CachedDefaultFragShader{};

    public:
        explicit ShaderImage(const std::shared_ptr<luvk::Renderer>& Renderer);
        ~ShaderImage();

        void RecordCommands(VkCommandBuffer Cmd) const;
        void UpdateImmediate(float DeltaTime);

        [[nodiscard]] bool Compile(const std::string& Source, std::string& OutError);
        void               Reset();

        [[nodiscard]] VkDescriptorSet GetDescriptorSet() const
        {
            return m_DescriptorSet->GetHandle();
        }

        static std::string_view GetDefaultSource();

    private:
        void Initialize();
        void CreatePipeline(const std::vector<std::uint32_t>& FragSpirv);
    };
}
