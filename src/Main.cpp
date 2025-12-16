// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include <cstdio>
#include <exception>
#include "luvk_example/Application/Application.hpp"

#ifdef main
#undef main
#endif

using namespace luvk_example;

std::int32_t main()
{
    try
    {
        Application& AppInstance = Application::GetInstance();

        do {}
        while (AppInstance.Render());

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception)
    {
        std::printf("Error: %s\n", Exception.what());
    }

    return EXIT_FAILURE;
}
