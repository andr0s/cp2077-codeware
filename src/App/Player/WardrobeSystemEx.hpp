#pragma once

#include "App/MacOSUnavailable.hpp"
#include "Red/TweakDB.hpp"
#include "Red/WardrobeSystem.hpp"

namespace App
{
struct WardrobeSystemEx : Red::WardrobeSystem
{
    bool ForgetItemID(const Red::ItemID& aItemID)
    {
#if defined(__APPLE__)
        // macOS: the wardrobe item store offsets (+0x48, lock +0xF4) are not verified on macOS.
        static std::once_flag s_reported;
        ReportUnavailableOnMacOS(s_reported, "WardrobeSystem.ForgetItemID");
        return false;
#endif
        auto& store = Raw::WardrobeSystem::ItemStore::Ref(this);
        auto& mutex = Raw::WardrobeSystem::ItemStoreMutex::Ref(this);
        auto key = Red::GetFlatValue<Red::CName>({aItemID.tdbid, ".appearanceName"});

        std::unique_lock _(mutex);
        auto success = store.Remove(key);

        if (!success)
        {
            Core::Vector<Red::CName> otherKeys;
            store.ForEach([&otherKeys, &aItemID](const Red::CName& aStoreKey, const Red::ItemID& aStoreItemID)
                          {
                              if (aItemID.tdbid == aStoreItemID.tdbid)
                              {
                                  otherKeys.push_back(aStoreKey);
                              }
                          });

            for (const auto& otherKey : otherKeys)
            {
                if (store.Remove(otherKey))
                {
                    success = true;
                }
            }
        }

        return success;
    }
};
}

RTTI_EXPAND_CLASS(Red::WardrobeSystem, App::WardrobeSystemEx, {
    RTTI_METHOD(ForgetItemID);
});
