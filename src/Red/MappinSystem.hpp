#pragma once

namespace Red
{
struct MappinData
{
    uint64_t id;
    Handle<IMappin> instance;
};
}

namespace Raw::MappinSystem
{
using MappinsLock = Core::OffsetPtr<0x198, Red::SharedSpinLock>;
using MappinsData = Core::OffsetPtr<0x1A0, Red::SortedArray<Red::MappinData>>;

constexpr auto SetPoiMappinPhase = Core::RawVFunc<
    /* addr = */ 0x1D0,
    /* type = */ void (Red::gamemappinsIMappinSystem::*)(const uint32_t aJournalPath,
                                                         const Red::gamedataMappinPhase aMappinPhase)>();

#if defined(__APPLE__)
// macOS: slot +0x438 (0x1042AA530, gamemappinsMappinSystem vtable 0x1072D9888); +0x3D0 is an unrelated flag setter.
constexpr auto GetPoiMappinPhase = Core::RawVFunc<
    /* addr = */ 0x430,
#else
constexpr auto GetPoiMappinPhase = Core::RawVFunc<
    /* addr = */ 0x3C8,
#endif
    /* type = */ Red::gamedataMappinPhase (Red::gamemappinsIMappinSystem::*)(const uint32_t aJournalPath)>();
}

namespace Raw::PointOfInterestMappin
{
#if defined(__APPLE__)
// macOS: clang packs the mappin's first member into the IMappin tail padding (+0x85), so the hash is at +0x88
// (POI constructor 0x1042EA968, SetPhase 0x1042EAF58).
using JournalHash = Core::OffsetPtr<0x88, uint32_t>;
#else
using JournalHash = Core::OffsetPtr<0x8C, uint32_t>;
#endif

constexpr auto SetPhase = Core::RawFunc<
    /* addr = */ Red::AddressLib::PointOfInterestMappin_SetPhase,
    /* type = */ bool (*)(void* aMappin, Red::gamedataMappinPhase aMappinPhase)>();
}
