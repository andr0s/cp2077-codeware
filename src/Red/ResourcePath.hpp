#pragma once

namespace Raw::ResourcePath
{
#if defined(__APPLE__)
// macOS: the StringView is passed by value (x0 = data, w1 = length) and the hash comes back in x0.
constexpr auto Create = Core::RawFunc<
    /* addr = */ Red::AddressLib::ResourcePath_Create,
    /* type = */ uint64_t (*)(const char* aPathStr, uint32_t aLength)>();
#else
constexpr auto Create = Core::RawFunc<
    /* addr = */ Red::AddressLib::ResourcePath_Create,
    /* type = */ Red::ResourcePath* (*)(Red::ResourcePath* aOut, Red::StringView* aPathStr)>();
#endif
}
