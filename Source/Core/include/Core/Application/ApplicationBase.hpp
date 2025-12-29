/*
 * Author: Lucas Vilas-Boas
 * Year: 2025
 * Repo: https://github.com/lucoiso/luvk_example
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <luvk/Modules/Renderer.hpp>
#include <Core/Components/InputManager.hpp>
#include <Core/UserInterface/ImGuiLayerBase.hpp>

class SDL_Window;

namespace Core
{
    class CORE_API ApplicationBase
    {
    protected:
        bool         m_CanRender{false};
        bool         m_ResizePending{false};
        std::int32_t m_Width{};
        std::int32_t m_Height{};
        float        m_DeltaTime{0.F};
        std::string  m_Title{};

        SDL_Window*                     m_Window{};
        VkSurfaceKHR                    m_Surface{VK_NULL_HANDLE};
        std::shared_ptr<InputManager>   m_Input;
        std::unique_ptr<ImGuiLayerBase> m_ImGuiLayer;
        std::shared_ptr<luvk::Renderer> m_Renderer{};
        std::vector<EventHandle>        m_EventHandles{};
        luvk::EventHandle               m_SurfaceDeleteHandle{};

    public:
        explicit ApplicationBase(std::uint32_t Width, std::uint32_t Height, std::uint32_t Flags);
        virtual  ~ApplicationBase();

        virtual bool Initialize();
        virtual void Shutdown();
        virtual bool Render();

        template <typename ImGuiLayerType, typename... Args> requires std::is_base_of_v<ImGuiLayerBase, ImGuiLayerType>
        void RegisterImGuiLayer(Args&&... InArguments)
        {
            m_ImGuiLayer = std::make_unique<ImGuiLayerType>(m_Window, m_Renderer, std::forward<Args>(InArguments)...);
            m_ImGuiLayer->PushStyle();
        }

        [[nodiscard]] constexpr std::uint32_t GetWidth() const noexcept
        {
            return static_cast<std::uint32_t>(m_Width);
        }

        [[nodiscard]] constexpr std::uint32_t GetHeight() const noexcept
        {
            return static_cast<std::uint32_t>(m_Height);
        }

        void SetTitle(std::string_view Title) noexcept;

        [[nodiscard]] constexpr std::string_view GetTitle() const noexcept
        {
            return m_Title;
        }

        [[nodiscard]] constexpr float GetDeltaTime() const noexcept
        {
            return m_DeltaTime;
        }

        [[nodiscard]] std::shared_ptr<InputManager> GetInput() const noexcept
        {
            return m_Input;
        }

        [[nodiscard]] constexpr SDL_Window* GetWindow() const noexcept
        {
            return m_Window;
        }

    protected:
        virtual bool OnBeginFrame(VkCommandBuffer CommandBuffer, std::uint32_t CurrentFrame);
        virtual bool OnRecordFrame(VkCommandBuffer CommandBuffer, std::uint32_t CurrentFrame);
        virtual void UserEventCallback(const SDL_Event& Event);

    private:
        void               RegisterInputBindings();
        void               UpdateExtensionsAndValidation() const;
        [[nodiscard]] bool InitializeRenderingCore();
    };
}
