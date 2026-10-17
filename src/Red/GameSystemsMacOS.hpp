#pragma once

#if defined(__APPLE__)
// macOS (arm64, Itanium C++ ABI): the SDK declares these game-system virtuals with the MSVC out-parameter convention
// (the result reference is the first argument). The macOS game returns the result directly instead (through x8 for
// Handle/SharedPtr, in w0 for IDs), so they are called through their vtable slots with the real signatures.
// RawVFunc takes MSVC-style offsets and adds 8 on macOS; each comment names the macOS slot and its 2.3.1 function.
// All other virtuals of these interfaces that Codeware uses match the SDK declarations on macOS.

namespace Raw::PopulationSystem
{
// gamePopulationSystem vtable 0x106F36420, slot +0x1B8 -> 0x10155C9D4 (handle written to [x8], id in w1)
constexpr auto FindEntity = Core::RawVFunc<
    /* addr = */ 0x1B0,
    /* type = */ Red::Handle<Red::Entity> (*)(Red::IPopulationSystem* aSystem, uint32_t aEntityID)>();
}

namespace Raw::EntityStubSystem
{
// gameEntityStubSystem vtable 0x106FBFDB8, slot +0x1B8 -> 0x101FD71BC (token written to [x8], callback ops at +0x80)
constexpr auto CreateStub = Core::RawVFunc<
    /* addr = */ 0x1B0,
    /* type = */ Red::EntityStubTokenPtr (*)(Red::IEntityStubSystem* aSystem, Red::EntityStubCreateRequest& aRequest,
                                             Red::EntityStubCallback&& aCallback)>();
}

namespace Raw::DynamicEntityIDSystem
{
// gameDynamicEntityIDSystem vtable 0x1071DD9A0, slot +0x1B8 -> 0x1035EDD9C (atomic counter +0xE0, id in w0)
constexpr auto GetNextTransientID = Core::RawVFunc<
    /* addr = */ 0x1B0,
    /* type = */ uint32_t (*)(Red::IDynamicEntityIDSystem* aSystem)>();

// gameDynamicEntityIDSystem vtable 0x1071DD9A0, slot +0x1C0 -> 0x1035EDDD8 (atomic counter +0xE4, id in w0)
constexpr auto GetNextPersistableID = Core::RawVFunc<
    /* addr = */ 0x1B8,
    /* type = */ uint32_t (*)(Red::IDynamicEntityIDSystem* aSystem)>();
}

namespace Raw::PersistencySystem
{
// gamePersistencySystem vtable 0x10729DE70, slot +0x1B8 -> 0x103FED0B4 (state written to [x8])
constexpr auto GetPersistentState = Core::RawVFunc<
    /* addr = */ 0x1B0,
    /* type = */ Red::Handle<Red::game::PersistentState> (*)(Red::IPersistencySystem* aSystem,
                                                             const Red::game::PersistentID& aPersistentID,
                                                             Red::CClass* aPersistentStateType, bool aCreateState)>();
}
#endif
