// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/Application.hpp"

#ifdef main
#undef main
#endif

using namespace luvk_example;

std::int32_t main()
{
    try
    {
        Application& AppInstance = Application::GetInstance();

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        while (AppInstance.Render());

        return EXIT_SUCCESS;
    }
    catch (const std::exception& Exception)
    {
        std::printf("Error: %s\n", Exception.what());
    }

    return EXIT_FAILURE;
}
