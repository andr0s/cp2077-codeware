#pragma once

namespace Raw::ResourceBank
{
#if defined(__APPLE__)
// macOS: the removed resource handle is returned through x8 and the path is the second argument (0x1021B4AB8).
constexpr auto ForgetResource = Core::RawFunc<
    /* addr = */ Red::AddressLib::ResourceBank_ForgetResource,
    /* type = */ Red::Handle<Red::CResource> (*)(uintptr_t aBank, const Red::ResourcePath aPath)>{};
#else
constexpr auto ForgetResource = Core::RawFunc<
    /* addr = */ Red::AddressLib::ResourceBank_ForgetResource,
    /* type = */ uintptr_t (*)(uintptr_t aBank, Red::Handle<Red::CResource>& aResource,
                               const Red::ResourcePath aPath)>{};
#endif
}
