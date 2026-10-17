#include "PersistencyService.hpp"
#include "Core/Facades/Log.hpp"

void App::PersistencyService::OnBootstrap()
{
#if defined(__APPLE__)
    if (!HookBefore<Raw::PersistentObject::InitializeState>(&OnInitializeState))
        Core::Log::Warning("[macOS] Merging persistent device states of dynamic entities is not available on macOS.");
#else
    HookBefore<Raw::PersistentObject::InitializeState>(&OnInitializeState).OrThrow();
#endif
}

void App::PersistencyService::OnInitializeState(Red::PersistentObject* aObject, Red::PersistentStateParams* aParams)
{
    if (aParams->persistentId.entityID.IsPersistable())
    {
        static const auto s_deviceComponentType = Red::GetClass("gameDeviceComponent");

        if (aParams->objectType->IsA(s_deviceComponentType))
        {
            aParams->mergeStates = true;
        }
    }
}
