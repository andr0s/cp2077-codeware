#include "OpenWorldTracker.hpp"
#include "Red/JournalManager.hpp"

void App::OpenWorldTracker::OnBootstrap()
{
#if defined(__APPLE__)
    if (!HookBefore<Raw::PointOfInterestMappin::SetPhase>(&OnSetPoiMappinPhase))
        LogWarning("[macOS] Open world activity tracking (point of interest phases) is not available on macOS.");
#else
    HookBefore<Raw::PointOfInterestMappin::SetPhase>(&OnSetPoiMappinPhase);
#endif
}

void App::OpenWorldTracker::OnSetPoiMappinPhase(void* aMappin, Red::gamedataMappinPhase aMappinPhase)
{
    if (aMappinPhase == Red::gamedataMappinPhase::CompletedPhase)
    {
        auto journalManager = Red::GetGameSystem<Red::gameIJournalManager>();
        auto& journalHash = Raw::PointOfInterestMappin::JournalHash::Ref(aMappin);

        Red::Handle<Red::gameJournalEntry> entry;
        Raw::JournalManager::GetEntryByHash(journalManager, entry, journalHash);

        if (entry)
        {
            Raw::JournalManager::ChangeEntryState(journalManager, entry, Red::gameJournalEntryState::Inactive,
                                                  Red::gameJournalNotifyOption::DoNotNotify, 1);
            Raw::JournalManager::ChangeEntryState(journalManager, entry, Red::gameJournalEntryState::Active,
                                                  Red::gameJournalNotifyOption::DoNotNotify, 1);
        }
    }
}
