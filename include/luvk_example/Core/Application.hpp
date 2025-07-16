// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/Core/InputManager.hpp"
#include <SDL2/SDL.h>

#include <memory>

#include <luvk/Core/Renderer.hpp>
#include <luvk/Core/Debug.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/Memory.hpp>
#include <luvk/Core/CommandPool.hpp>
#include <luvk/Core/Synchronization.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/ThreadPool.hpp>

namespace luvk_example
{
    class Application
    {
        InputManager m_Input;
        SDL_Window* m_Window{};
        std::uint32_t m_Width{};
        std::uint32_t m_Height{};
        std::shared_ptr<luvk::Renderer> m_Renderer{};
        std::shared_ptr<luvk::Debug> m_DebugModule{};
        std::shared_ptr<luvk::SwapChain> m_SwapChainModule{};
        std::shared_ptr<luvk::Device> m_DeviceModule{};
        std::shared_ptr<luvk::Memory> m_MemoryModule{};
        std::shared_ptr<luvk::CommandPool> m_CommandPoolModule{};
        std::shared_ptr<luvk::Synchronization> m_SynchronizationModule{};
        std::shared_ptr<luvk::MeshRegistry> m_MeshRegistryModule{};
        std::shared_ptr<luvk::ThreadPool> m_ThreadPoolModule{};

    public:
        Application(std::uint32_t Width, std::uint32_t Height);
        ~Application();

        bool Initialize();

        [[nodiscard]] InputManager& GetInput() noexcept
        {
            return m_Input;
        }

        [[nodiscard]] std::shared_ptr<luvk::Renderer> const& GetRenderer() const noexcept
        {
            return m_Renderer;
        }

        [[nodiscard]] std::shared_ptr<luvk::Device> const& GetDevice() const noexcept
        {
            return m_DeviceModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::SwapChain> const& GetSwapChain() const noexcept
        {
            return m_SwapChainModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::CommandPool> const& GetCommandPool() const noexcept
        {
            return m_CommandPoolModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::Memory> const& GetMemory() const noexcept
        {
            return m_MemoryModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::Synchronization> const& GetSync() const noexcept
        {
            return m_SynchronizationModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::MeshRegistry> const& GetMeshRegistry() const noexcept
        {
            return m_MeshRegistryModule;
        }

        [[nodiscard]] std::shared_ptr<luvk::ThreadPool> const& GetThreadPool() const noexcept
        {
            return m_ThreadPoolModule;
        }

        [[nodiscard]] SDL_Window* GetWindow() const noexcept
        {
            return m_Window;
        }
    };
} // namespace luvk_example
