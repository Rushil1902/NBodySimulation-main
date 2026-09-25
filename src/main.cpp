#include "application/Application.hpp"
#include "core/AssetPaths.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        AssetPaths::initialize(argc > 0 ? argv[0] : nullptr);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Failed to locate application assets: " << error.what() << '\n';
        return -1;
    }

    Application application;
    return application.run();
}
