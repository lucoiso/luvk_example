// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/Core/ApplicationBase.hpp"
#include "luvk_example/Core/Camera.hpp"
#include "luvk_example/Meshes/Cube.hpp"
#include "luvk_example/Meshes/Pixel.hpp"
#include "luvk_example/Meshes/Triangle.hpp"
#include "luvk_example/UserInterface/ImGuiLayer.hpp"

namespace luvk_example
{
    class Application : public ApplicationBase
    {
        bool                      m_CanRender{true};
        bool                      m_ResizePending{false};
        std::unique_ptr<Cube>     m_CubeMesh;
        std::unique_ptr<Triangle> m_TriangleMesh;
        std::unique_ptr<Pixel>    m_PixelMesh;
        ImGuiLayer                m_ImGuiLayer;
        Camera                    m_Camera;

        Application();
        ~Application() override;
        bool Initialize() override;

    public:
        static Application& GetInstance();
        bool                Render() override;

        [[nodiscard]] constexpr ImGuiLayer& GetImGuiLayer() noexcept
        {
            return m_ImGuiLayer;
        }

        [[nodiscard]] constexpr Camera& GetCamera() noexcept
        {
            return m_Camera;
        }

    private:
        void CreateScene();
        void RegisterInputBindings();
    };
} // namespace luvk_example
