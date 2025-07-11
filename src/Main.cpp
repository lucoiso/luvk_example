// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Application.hpp"

#ifdef main
#undef main
#endif

using namespace luvk_example;

int main()
{
    constexpr std::uint32_t WindowWidth = 800U;
    constexpr std::uint32_t WindowHeight = 600U;

    Application App(WindowWidth, WindowHeight);
    if (!App.Initialize())
    {
        return EXIT_FAILURE;
    }

    App.Run();
    return EXIT_SUCCESS;
}

