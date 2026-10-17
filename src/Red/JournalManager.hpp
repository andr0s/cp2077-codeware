#pragma once

namespace Red
{
using JournalEntryHash = uint32_t;
}

namespace Raw::JournalManager
{
constexpr auto GetEntries = Core::RawVFunc<
    /* addr = */ 0x210,
    /* type = */ void* (Red::gameIJournalManager::*)(const Red::gameJournalRequestContext& aRequest,
                                                     Red::DynArray<Red::WeakHandle<Red::gameJournalEntry>>& aOut)>();

#if defined(__APPLE__)
// macOS (gameJournalManager vtable 0x106FA5A90): MSVC orders overloaded virtuals in reverse, so these two are not at
// MSVC+8. GetEntryByHash is slot +0x220 (0x101EB3214) and returns the entry through x8; ChangeEntryState(entry) is
// slot +0x268 (0x101EB378C). RawVFunc adds 8 to the offsets below.
constexpr auto GetEntryByHash = Core::RawVFunc<
    /* addr = */ 0x218,
    /* type = */ Red::Handle<Red::gameJournalEntry> (Red::gameIJournalManager::*)(Red::JournalEntryHash aEntryHash)>();

constexpr auto ChangeEntryState = Core::RawVFunc<
    /* addr = */ 0x260,
#else
constexpr auto GetEntryByHash = Core::RawVFunc<
    /* addr = */ 0x220,
    /* type = */ void* (Red::gameIJournalManager::*)(Red::Handle<Red::gameJournalEntry>& aOut,
                                                     Red::JournalEntryHash aEntryHash)>();

constexpr auto ChangeEntryState = Core::RawVFunc<
    /* addr = */ 0x268,
#endif
    /* type = */ bool (Red::gameIJournalManager::*)(Red::Handle<Red::gameJournalEntry>& aEntry,
                                                    Red::gameJournalEntryState aEntryState,
                                                    Red::gameJournalNotifyOption aNotifyOption,
                                                    uint32_t a4)>();

constexpr auto GetEntryState = Core::RawVFunc<
    /* addr = */ 0x2B0,
    /* type = */ Red::gameJournalEntryState (Red::gameIJournalManager::*)(Red::Handle<Red::gameJournalEntry>& aEntry)>();

constexpr auto GetEntryTimestamp = Core::RawVFunc<
    /* addr = */ 0x2C8,
    /* type = */ uint32_t (Red::gameIJournalManager::*)(Red::Handle<Red::gameJournalEntry>& aEntry)>();
}
