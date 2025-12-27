/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <Core/Application/ApplicationBase.hpp>
#include <volk.h>

namespace UserInterface
{
    class Camera;
    class Cube;
    class Triangle;
    class Pixel;

    class USERINTERFACE_API Application : public Core::ApplicationBase
    {
        std::unique_ptr<Cube>     m_CubeMesh;
        std::unique_ptr<Triangle> m_TriangleMesh;
        std::unique_ptr<Pixel>    m_PixelMesh;
        std::shared_ptr<Camera>   m_Camera;

        static std::shared_ptr<Application> s_Instance;

        explicit Application(std::uint32_t Width, std::uint32_t Height);

    public:
        Application()           = delete;
        ~Application() override = default;

        static std::shared_ptr<Application> GetInstance();

        bool Initialize() override;
        void Shutdown() override;

        [[nodiscard]] std::shared_ptr<Camera> GetCamera() const noexcept
        {
            return m_Camera;
        }

    private:
        void CreateScene();

    protected:
        bool OnBeginFrame(VkCommandBuffer CommandBuffer) override;
        bool OnRecordFrame(VkCommandBuffer CommandBuffer, std::uint32_t CurrentFrame) override;
        void UserEventCallback(const SDL_Event& Event) override;
        void SetupDeviceExtensions() const override;
        [[nodiscard]] void* GetDeviceFeatureChain() const override;
    };
}
