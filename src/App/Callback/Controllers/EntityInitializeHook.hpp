#pragma once

#include "App/Callback/CallbackSystem.hpp"
#include "App/Callback/CallbackSystemController.hpp"
#include "App/Callback/Events/EntityLifecycleEvent.hpp"
#include "Core/Hooking/HookingAgent.hpp"
#include "Red/Entity.hpp"
#include "Red/Puppet.hpp"
#include "Red/Vehicle.hpp"

namespace App
{
class EntityInitializeHook
    : public CallbackSystemController
    , public Core::HookingAgent
{
public:
    constexpr static auto EventName = Red::CName("Entity/Initialize");

    Core::Map<Red::CName, Red::CName> GetEvents() override
    {
        return {{EventName, Red::GetTypeName<EntityLifecycleEvent>()}};
    }

protected:
    bool OnActivateHook() override
    {
        return IsHooked<Raw::Entity::Initialize>() || HookBefore<Raw::Entity::Initialize>(&OnInitialize);
    }

    bool OnDeactivateHook() override
    {
        return !IsHooked<Raw::Entity::Initialize>() || Unhook<Raw::Entity::Initialize>();
    }

    inline static void OnInitialize(Red::Entity* aEntity, Red::JobQueue& aJobQueue,
                                    Red::EntityInitializeRequest* aRequest)
    {
        if (aRequest->entityID)
        {
            Raw::Entity::EntityID::Set(aEntity, aRequest->entityID);
        }

#if defined(__APPLE__)
        // As upstream's EntityRequestComponentsHook does: resolve the record ID before the event, so that record ID
        // filters match during "Entity/Initialize".
        if (Red::Cast<Red::gamePuppetBase>(aEntity))
        {
            Raw::Puppet::RecordID::Ref(aEntity) = Raw::Entity::ResolveRecordID(aEntity, aRequest->populationParam);
        }
        else if (Red::Cast<Red::vehicleBaseObject>(aEntity))
        {
            Raw::Vehicle::RecordID::Ref(aEntity) = Raw::Entity::ResolveRecordID(aEntity, aRequest->populationParam);
        }
#endif

        CallbackSystem::Get()->DispatchNativeEvent<EntityLifecycleEvent>(EventName, Red::AsWeakHandle(aEntity));
    }
};
}
