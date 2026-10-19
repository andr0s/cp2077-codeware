#include "WidgetSpawningService.hpp"
#include "App/Callback/CallbackSystem.hpp"
#include "App/Callback/Events/InkWidgetSpawnEvent.hpp"

namespace
{
constexpr auto ControllerSeparator = ':';

Red::ClassLocator<Red::ink::IWidgetController> s_gameControllerType;
Red::ClassLocator<Red::ink::WidgetLogicController> s_logicControllerType;
}

void App::WidgetSpawningService::OnBootstrap()
{
#if defined(__APPLE__)
    // Off on macOS for now: the game crashed in its ink spawn jobs with Codeware's UI hooks in place, and none of the
    // mods in use (BT-7274, REDCheatMenu2) needs them.
    LogInfo("[macOS] Widget spawning hooks are off.");
#else
    Hook<Raw::InkWidgetLibrary::SpawnFromLocal>(&OnSpawnLocal).OrThrow();
    Hook<Raw::InkWidgetLibrary::SpawnFromExternal>(&OnSpawnExternal).OrThrow();
    Hook<Raw::InkWidgetLibrary::AsyncSpawnFromLocal>(&OnAsyncSpawnLocal).OrThrow();
    Hook<Raw::InkWidgetLibrary::AsyncSpawnFromExternal>(&OnAsyncSpawnExternal).OrThrow();
    HookBefore<Raw::InkSpawner::FinishAsyncSpawn>(&OnFinishAsyncSpawn).OrThrow();
#endif
}

void App::WidgetSpawningService::OnShutdown()
{
    Unhook<Raw::InkWidgetLibrary::SpawnFromLocal>();
    Unhook<Raw::InkWidgetLibrary::SpawnFromExternal>();
    Unhook<Raw::InkWidgetLibrary::AsyncSpawnFromLocal>();
    Unhook<Raw::InkWidgetLibrary::AsyncSpawnFromExternal>();
    Unhook<Raw::InkSpawner::FinishAsyncSpawn>();
}

#if defined(__APPLE__)
Red::Handle<Red::ink::WidgetLibraryItemInstance> App::WidgetSpawningService::OnSpawnLocal(
    Red::ink::WidgetLibraryResource& aLibrary, Red::CName aItemName)
{
    auto instance = Raw::InkWidgetLibrary::SpawnFromLocal(aLibrary, aItemName);

    if (!instance)
    {
        auto* itemNameStr = aItemName.ToString();
        auto* controllerSep = strchr(itemNameStr, ControllerSeparator);

        if (controllerSep)
        {
            aItemName = Red::CName(itemNameStr, controllerSep - itemNameStr);
            instance = Raw::InkWidgetLibrary::SpawnFromLocal(aLibrary, aItemName);

            if (instance)
            {
                InjectController(instance, controllerSep + 1);
            }
        }
    }

    if (s_widgetSpawnEventEnabled && instance)
    {
        CallbackSystem::Get()->DispatchNativeEvent<inkWidgetSpawnEvent>(WidgetSpawnEventName, aLibrary.path, aItemName,
                                                                        instance);
    }

    return instance;
}

Red::Handle<Red::ink::WidgetLibraryItemInstance> App::WidgetSpawningService::OnSpawnExternal(
    Red::ink::WidgetLibraryResource& aLibrary, Red::ResourcePath aExternalPath, Red::CName aItemName)
{
    if (!aExternalPath || IsDependency(aLibrary, aExternalPath))
    {
        return Raw::InkWidgetLibrary::SpawnFromExternal(aLibrary, aExternalPath, aItemName);
    }

    auto* externalLibrary = LoadDependency(aExternalPath);

    if (!externalLibrary)
    {
        return {};
    }

    return Raw::InkWidgetLibrary::SpawnFromLocal(*externalLibrary, aItemName);
}

bool App::WidgetSpawningService::OnAsyncSpawnLocal(Red::ink::WidgetLibraryResource& aLibrary,
                                                   Red::InkSpawningInfo& aSpawningInfo,
                                                   Red::CName aItemName,
                                                   bool a4)
{
    auto* itemNameStr = aItemName.ToString();
    auto* controllerSep = strchr(itemNameStr, ControllerSeparator);

    if (controllerSep)
    {
        aItemName = Red::CName(itemNameStr, controllerSep - itemNameStr);
    }

    return Raw::InkWidgetLibrary::AsyncSpawnFromLocal(aLibrary, aSpawningInfo, aItemName, a4);
}

bool App::WidgetSpawningService::OnAsyncSpawnExternal(Red::ink::WidgetLibraryResource& aLibrary,
                                                      Red::InkSpawningInfo& aSpawningInfo,
                                                      Red::ResourcePath aExternalPath,
                                                      Red::CName aItemName,
                                                      bool a5)
{
    if (!aExternalPath || IsDependency(aLibrary, aExternalPath))
    {
        return Raw::InkWidgetLibrary::AsyncSpawnFromExternal(aLibrary, aSpawningInfo, aExternalPath, aItemName, a5);
    }

    auto* externalLibrary = LoadDependency(aExternalPath);

    return externalLibrary &&
           Raw::InkWidgetLibrary::AsyncSpawnFromLocal(*externalLibrary, aSpawningInfo, aItemName, a5);
}

// Windows Codeware appends a library that aLibrary does not list to aLibrary.externalLibraries (InjectDependency).
// Here the dispatcher threads spawn from the same library at the same time and walk that array without a lock; the
// game's lookup (0x104965C90) also re-reads its end after the search. An append there made them read past the array
// or freed memory (crash in the ink spawn jobs). So the game's array is never changed: an unlisted library is loaded
// and kept here, and the item is spawned from it as a local one. That is what the game's external spawns do once they
// have found the library (0x104965B38, 0x104965EC0).
bool App::WidgetSpawningService::IsDependency(Red::ink::WidgetLibraryResource& aLibrary,
                                              Red::ResourcePath aExternalPath)
{
    for (const auto& externalLibrary : aLibrary.externalLibraries)
    {
        if (externalLibrary.path == aExternalPath)
        {
            return true;
        }
    }

    return false;
}

Red::ink::WidgetLibraryResource* App::WidgetSpawningService::LoadDependency(Red::ResourcePath aExternalPath)
{
    // Never destroyed: at exit the game's resource system is gone before static destructors would run.
    static auto* s_libraries = new Core::Map<uint64_t, Red::Ref<Red::ink::WidgetLibraryResource>>();

    std::unique_lock _(s_mutex);

    auto& externalLibrary = (*s_libraries)[aExternalPath.hash];
    const auto isNew = !externalLibrary.token;

    if (isNew)
    {
        externalLibrary.path = aExternalPath;
        externalLibrary.LoadAsync();
    }

    Red::WaitForResource(externalLibrary, std::chrono::milliseconds(1000));

    if (isNew)
    {
        LogInfo("[WidgetSpawningService] External library {:016X}: {}", aExternalPath.hash,
                externalLibrary.IsLoaded() ? "loaded" : "not loaded");
    }

    return externalLibrary.IsLoaded() ? externalLibrary.Get().instance : nullptr;
}
#else
uintptr_t App::WidgetSpawningService::OnSpawnLocal(Red::ink::WidgetLibraryResource& aLibrary,
                                                   Red::Handle<Red::ink::WidgetLibraryItemInstance>& aInstance,
                                                   Red::CName aItemName)
{
    auto result = Raw::InkWidgetLibrary::SpawnFromLocal(aLibrary, aInstance, aItemName);

    if (!aInstance)
    {
        auto* itemNameStr = aItemName.ToString();
        auto* controllerSep = strchr(itemNameStr, ControllerSeparator);

        if (controllerSep)
        {
            aItemName = Red::CName(itemNameStr, controllerSep - itemNameStr);
            Raw::InkWidgetLibrary::SpawnFromLocal(aLibrary, aInstance, aItemName);

            if (aInstance)
            {
                InjectController(aInstance, controllerSep + 1);
            }
        }
    }

    if (s_widgetSpawnEventEnabled && aInstance)
    {
        CallbackSystem::Get()->DispatchNativeEvent<inkWidgetSpawnEvent>(WidgetSpawnEventName, aLibrary.path, aItemName,
                                                                        aInstance);
    }

    return result;
}

uintptr_t App::WidgetSpawningService::OnSpawnExternal(Red::ink::WidgetLibraryResource& aLibrary,
                                                      Red::Handle<Red::ink::WidgetLibraryItemInstance>& aInstance,
                                                      Red::ResourcePath aExternalPath,
                                                      Red::CName aItemName)
{
    InjectDependency(aLibrary, aExternalPath);

    return Raw::InkWidgetLibrary::SpawnFromExternal(aLibrary, aInstance, aExternalPath, aItemName);
}

bool App::WidgetSpawningService::OnAsyncSpawnLocal(Red::ink::WidgetLibraryResource& aLibrary,
                                                   Red::InkSpawningInfo& aSpawningInfo,
                                                   Red::CName aItemName)
{
    auto* itemNameStr = aItemName.ToString();
    auto* controllerSep = strchr(itemNameStr, ControllerSeparator);

    if (controllerSep)
    {
        aItemName = Red::CName(itemNameStr, controllerSep - itemNameStr);
    }

    return Raw::InkWidgetLibrary::AsyncSpawnFromLocal(aLibrary, aSpawningInfo, aItemName);
}

bool App::WidgetSpawningService::OnAsyncSpawnExternal(Red::ink::WidgetLibraryResource& aLibrary,
                                                      Red::InkSpawningInfo& aSpawningInfo,
                                                      Red::ResourcePath aExternalPath,
                                                      Red::CName aItemName)
{
    InjectDependency(aLibrary, aExternalPath);

    return Raw::InkWidgetLibrary::AsyncSpawnFromExternal(aLibrary, aSpawningInfo, aExternalPath, aItemName);
}
#endif

void App::WidgetSpawningService::OnFinishAsyncSpawn(Red::InkSpawningContext& aContext,
                                                    Red::Handle<Red::ink::WidgetLibraryItemInstance>& aInstance)
{
    auto* itemNameStr = aContext.request->itemName.ToString();
    auto* controllerSep = strchr(itemNameStr, ControllerSeparator);

    if (controllerSep)
    {
        InjectController(aInstance, controllerSep + 1);
    }

    if (s_widgetSpawnEventEnabled)
    {
        auto libraryPath = aContext.request->externalLibrary
                               ? aContext.request->externalLibrary
                               : aContext.request->library->path;
        auto itemName = controllerSep
                            ? Red::CName(itemNameStr, controllerSep - itemNameStr)
                            : aContext.request->itemName;

        CallbackSystem::Get()->DispatchNativeEvent<inkWidgetSpawnEvent>(WidgetSpawnEventName, libraryPath, itemName,
                                                                        aInstance);
    }
}

void App::WidgetSpawningService::InjectDependency(Red::ink::WidgetLibraryResource& aLibrary,
                                                  Red::ResourcePath aExternalPath)
{
    bool libraryExists = false;

    // Check if the external library is in the list and do nothing if it is
    {
        std::shared_lock _(s_mutex);
        for (const auto& externalLibrary : aLibrary.externalLibraries)
        {
            if (externalLibrary.path == aExternalPath)
            {
                libraryExists = true;
                break;
            }
        }
    }

    // Add the requested library to the list
    if (!libraryExists)
    {
        std::unique_lock _(s_mutex);
        aLibrary.externalLibraries.EmplaceBack(aExternalPath);

        // Load requested library for the spawner
        auto& externalLibrary = aLibrary.externalLibraries.Back();
        externalLibrary.LoadAsync();

        Red::WaitForResource(externalLibrary.token, std::chrono::milliseconds(1000));
    }
}

void App::WidgetSpawningService::InjectController(Red::Handle<Red::ink::WidgetLibraryItemInstance>& aInstance,
                                                Red::CName aControllerName)
{
    auto* controllerType = Red::CRTTISystem::Get()->GetClass(aControllerName);

    if (controllerType)
    {
        if (controllerType->IsA(s_gameControllerType))
        {
            auto* controllerInstance = reinterpret_cast<Red::ink::IWidgetController*>(controllerType->CreateInstance(true));
            Red::Handle<Red::ink::IWidgetController> controllerHandle(controllerInstance);

            aInstance->gameController.Swap(controllerHandle);

            if (controllerHandle.instance)
            {
                InheritProperties(controllerInstance, controllerHandle.instance);
            }
        }
        else if (controllerType->IsA(s_logicControllerType))
        {
            auto* controllerInstance = reinterpret_cast<Red::ink::WidgetLogicController*>(controllerType->CreateInstance(true));
            Red::Handle<Red::ink::WidgetLogicController> controllerHandle(controllerInstance);

            aInstance->rootWidget->logicController.Swap(controllerHandle);

            if (controllerHandle.instance)
            {
                InheritProperties(controllerInstance, controllerHandle.instance);
            }
        }
    }
}

void App::WidgetSpawningService::InheritProperties(Red::IScriptable* aTarget, Red::IScriptable* aSource)
{
    auto* sourceType = aSource->GetType();
    auto* targetType = aTarget->GetType();

    Red::DynArray<Red::CProperty*> sourceProps;
    sourceType->GetProperties(sourceProps);

    for (const auto& sourceProp : sourceProps)
    {
        const auto targetProp = targetType->GetProperty(sourceProp->name);

        if (targetProp && targetProp->type == sourceProp->type)
        {
            targetProp->SetValue(aTarget, sourceProp->GetValuePtr<void>(aSource));
        }
    }
}

void App::WidgetSpawningService::ToggleWidgetSpawnEvent(bool aState)
{
    s_widgetSpawnEventEnabled = aState;
}
