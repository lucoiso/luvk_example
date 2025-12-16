// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk
{
    class Pipeline;
    class Buffer;
    class DescriptorPool;
    class SwapChain;
}

namespace luvk_example
{
    class Cube : public luvk::Mesh
    {
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        mutable glm::mat4               m_Mvp{1.f};

    public:
        Cube(const std::shared_ptr<luvk::Device>&         Device,
             const std::shared_ptr<luvk::SwapChain>&      Swap,
             const std::shared_ptr<luvk::Memory>&         Memory,
             const std::shared_ptr<luvk::DescriptorPool>& Pool);

        void Tick(float DeltaTime) override;
    };
}
