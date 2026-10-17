#pragma once

namespace Raw::Localization
{
#if defined(__APPLE__)
// macOS: the handle is returned through x8 and the path is the first argument (x0).
constexpr auto LoadTexts = Core::RawFunc<
    /* addr = */ Red::AddressLib::Localization_LoadOnScreens,
    /* type = */ Red::Handle<Red::localization::PersistenceOnScreenEntries> (*)(Red::ResourcePath aPath)>();
#else
constexpr auto LoadTexts = Core::RawFunc<
    /* addr = */ Red::AddressLib::Localization_LoadOnScreens,
    /* type = */ uint64_t (*)(Red::Handle<Red::localization::PersistenceOnScreenEntries>& aOut,
                              Red::ResourcePath aPath)>();
#endif
}
