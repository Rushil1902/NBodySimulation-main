#pragma once

#include <filesystem>
#include <stdexcept>
#include <string>

namespace AssetPaths {
namespace detail {
inline std::filesystem::path& assetRootStorage()
{
    static std::filesystem::path assetRoot;
    return assetRoot;
}
}

inline void initialize(const char* executablePath)
{
    if (executablePath == nullptr || executablePath[0] == '\0')
        throw std::runtime_error("Cannot locate runtime assets: executable path is empty.");

    std::error_code error;
    std::filesystem::path executable = std::filesystem::absolute(
        std::filesystem::u8path(executablePath),
        error);
    if (error)
    {
        throw std::runtime_error(
            "Cannot resolve executable path '" + std::string(executablePath)
            + "': " + error.message());
    }

    const std::filesystem::path canonicalExecutable =
        std::filesystem::weakly_canonical(executable, error);
    if (!error)
        executable = canonicalExecutable;

    detail::assetRootStorage() = executable.parent_path() / "assets";
}

inline const std::filesystem::path& root()
{
    const std::filesystem::path& assetRoot = detail::assetRootStorage();
    if (assetRoot.empty())
        throw std::runtime_error("Runtime asset paths were used before initialization.");
    return assetRoot;
}

inline std::filesystem::path resolve(const std::filesystem::path& relativePath)
{
    const std::filesystem::path resolvedPath = root() / relativePath;
    std::error_code error;
    if (!std::filesystem::is_regular_file(resolvedPath, error))
    {
        throw std::runtime_error(
            "Required runtime asset was not found: " + resolvedPath.string());
    }
    return resolvedPath;
}
}
