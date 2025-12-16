// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/Application/ApplicationBase.hpp"
#include "luvk_example/Components/Camera.hpp"

namespace luvk_example
{
    class Cube;
    class Triangle;
    class Pixel;
    class ImGuiLayer;

    class Application : public ApplicationBase
    {
        Camera                      m_Camera;
        std::unique_ptr<Cube>       m_CubeMesh;
        std::unique_ptr<Triangle>   m_TriangleMesh;
        std::unique_ptr<Pixel>      m_PixelMesh;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

        Application() = delete;
        explicit Application(std::uint32_t Width, std::uint32_t Height);
        ~Application() override;

    public:
        static Application& GetInstance();

        [[nodiscard]] constexpr Camera& GetCamera() noexcept
        {
            return m_Camera;
        }

    private:
        void CreateScene();
        void RegisterInputBindings();
    };
}
