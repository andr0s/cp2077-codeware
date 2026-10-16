#include "ModuleImage.hpp"
#include <iostream>
#if !defined(_WIN32) && !defined(_WIN64)
#include <dlfcn.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
Core::ModuleImage::ModuleImage(HMODULE aHandle)
{
    std::wstring filePath;
    wil::GetModuleFileNameW(aHandle, filePath);

    m_path = filePath;
}
#else
// Helper function to get the path of this library
static std::string GetThisLibraryPath()
{
    Dl_info info;
    // Use the address of this function to find which library contains it
    if (dladdr(reinterpret_cast<void*>(&GetThisLibraryPath), &info) && info.dli_fname)
    {
        return info.dli_fname;
    }
    return "";
}

Core::ModuleImage::ModuleImage(void* /*aHandle*/)
{
    // On macOS the handle RED4ext passes is an opaque plugin handle, not an address inside this image, so dladdr on it
    // could name another image. Ask dyld about an address inside this library instead.
    std::string filePath = GetThisLibraryPath();

    if (!filePath.empty())
    {
        m_path = filePath;
    }
    else
    {
        std::cerr << "[ModuleImage] Warning: Could not determine module path" << std::endl;
        m_path = "";
    }
}
#endif

std::filesystem::path Core::ModuleImage::GetPath() const
{
    return m_path;
}

std::filesystem::path Core::ModuleImage::GetDir() const
{
    return m_path.parent_path();
}

std::string Core::ModuleImage::GetName() const
{
    return m_path.stem().string();
}

bool Core::ModuleImage::IsASI() const
{
#if defined(_WIN32) || defined(_WIN64)
    return m_path.extension() == L".asi";
#else
    return m_path.extension() == ".asi";
#endif
}
