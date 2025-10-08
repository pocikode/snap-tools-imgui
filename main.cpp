#include <iostream>
#include <memory>
#include "Application.h"

int main()
{
    try
    {
        auto app = std::make_unique<Application>();

        if (!app->Initialize())
        {
            std::cerr << "Failed to initialize application" << std::endl;
            return -1;
        }

        std::cout << "Starting cross-platform application..." << std::endl;
        app->Run();

        std::cout << "Application finished successfully" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
        return -1;
    }
}