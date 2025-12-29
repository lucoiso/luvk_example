/*
* Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Resources/Mesh.hpp>

namespace luvk
{
    class Pipeline;
    class Buffer;
    class SwapChain;
}

namespace UserInterface
{
    class Cube : public luvk::MeshShaderMesh
    {
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        mutable glm::mat4               m_Mvp{1.F};

    public:
        Cube(luvk::Device* Device, const luvk::SwapChain* Swap, luvk::Memory* Memory);
        void Tick(float DeltaTime) const;
    };
}
