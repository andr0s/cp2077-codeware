#pragma once

namespace Red
{
inline NodeRef ResolveNodeRef(NodeRef aNodeRef, GlobalNodeRef aContext = GlobalNodeRef::GlobalRoot)
{
    GlobalNodeRef resolved{};
    CallGlobal("ResolveNodeRef", resolved, aNodeRef, aContext);

    return resolved.hash;
}

inline EntityID ResolveEntityID(NodeRef aNodeRef, GlobalNodeRef aContext = GlobalNodeRef::GlobalRoot)
{
    return ResolveNodeRef(aNodeRef).hash;
}
}

namespace Raw::NodeRef
{
#if defined(__APPLE__)
// macOS: the string view is passed by value (x0 = data, w1 = length) and the hash comes back in x0 (0x1033409EC,
// an 8-byte thunk: call only).
constexpr auto Create = Core::RawFunc<
    /* addr = */ Red::AddressLib::NodeRef_Create,
    /* type = */ uint64_t (*)(const char* aReference, uint32_t aLength)>();
#else
constexpr auto Create = Core::RawFunc<
    /* addr = */ Red::AddressLib::NodeRef_Create,
    /* type = */ Red::NodeRef* (*)(Red::NodeRef& aOut, Red::StringView& aReference)>();
#endif
}
