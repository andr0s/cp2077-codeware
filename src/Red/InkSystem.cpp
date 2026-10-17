#include "InkSystem.hpp"

Red::InkSystem* Red::InkSystem::Get()
{
#if defined(__APPLE__)
    // macOS: the instance pointer's address may be unresolved (0); report no ink system instead of dereferencing it.
    if (!Raw::inkSystem::Instance)
        return nullptr;
#endif
    return Raw::inkSystem::Instance;
}

Red::InkLayerManager* Red::InkSystem::GetLayerManager()
{
    return layerManagers[0].instance;
}

Red::DynArray<Red::Handle<Red::inkLayer>>& Red::InkSystem::GetLayers()
{
    return layerManagers[0]->layers;
}

Red::WeakHandle<Red::ink::ISystemRequestsHandler>& Red::InkSystem::GetSystemRequestsHandler()
{
    return requestsHandler;
}

Red::ink::ISystemRequestsHandler* Red::InkSystem::GetSystemRequestsHandlerPtr()
{
    return requestsHandler.instance;
}
