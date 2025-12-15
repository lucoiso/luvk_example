// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "luvk_example/Components/InputManager.hpp"

namespace luvk
{
    class Renderer;
    class Debug;
    class SwapChain;
    class Device;
    class Memory;
    class CommandPool;
    class Synchronization;
    class ThreadPool;
    class DescriptorPool;
} // namespace luvk

namespace luvk_example
{
    class ApplicationBase
    {
    protected:
        InputManager  m_Input;
        SDL_Window*   m_Window{};
        std::uint32_t m_Width{};
        std::uint32_t m_Height{};

        std::shared_ptr<luvk::Renderer>        m_Renderer{};
        std::shared_ptr<luvk::Debug>           m_DebugModule{};
        std::shared_ptr<luvk::SwapChain>       m_SwapChainModule{};
        std::shared_ptr<luvk::Device>          m_DeviceModule{};
        std::shared_ptr<luvk::Memory>          m_MemoryModule{};
        std::shared_ptr<luvk::CommandPool>     m_CommandPoolModule{};
        std::shared_ptr<luvk::Synchronization> m_SynchronizationModule{};
        std::shared_ptr<luvk::ThreadPool>      m_ThreadPoolModule{};
        std::shared_ptr<luvk::DescriptorPool>  m_DescriptorPoolModule{};

    public:
        explicit ApplicationBase(std::uint32_t Width, std::uint32_t Height);
        virtual  ~ApplicationBase();

        virtual bool Initialize();
        virtual bool Render();

        [[nodiscard]] constexpr InputManager& GetInput() noexcept
        {
            return m_Input;
        }

        [[nodiscard]] constexpr SDL_Window* GetWindow() const noexcept
        {
            return m_Window;
        }

    private:
        void               RegisterModules();
        void               SetupExtensions() const;
        [[nodiscard]] bool InitializeModules() const;
        [[nodiscard]] bool InitializeDeviceModule() const;
    };
} // namespace luvk_example
