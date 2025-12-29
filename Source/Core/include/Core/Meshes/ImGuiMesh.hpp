/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <imgui.h>
#include <vector>
#include <luvk/Resources/Mesh.hpp>

namespace luvk
{
    class Device;
    class Memory;
    class DescriptorPool;
}

namespace Core
{
    class CORE_API ImGuiMesh : public luvk::Mesh
    {
        const ImDrawData*       m_ActiveDrawData{nullptr};
        std::vector<ImDrawVert> m_Vertices{};
        std::vector<ImDrawIdx>  m_Indices{};

    public:
        ImGuiMesh() = delete;

        explicit ImGuiMesh(luvk::Device* Device, luvk::Memory* Memory, luvk::DescriptorPool* Pool);
        explicit ImGuiMesh(luvk::Device* Device, luvk::Memory* Memory, const std::shared_ptr<luvk::Material>& FontMaterial);

        void UpdateBuffers(const ImDrawData* DrawData);
        void Render(VkCommandBuffer CommandBuffer) const override;
    };
}
