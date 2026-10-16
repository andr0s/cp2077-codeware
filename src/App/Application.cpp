#include "Application.hpp"
#include "App/Entity/PersistencyService.hpp"
#include "App/Environment.hpp"
#include "App/Localization/LocalizationService.hpp"
#include "App/Migration.hpp"
#include "App/Project.hpp"
#include "App/Quest/QuestPhaseRegistry.hpp"
#include "App/Scripting/ScriptingService.hpp"
#include "App/Shared/ResourcePathRegistry.hpp"
#include "App/UI/WidgetBuildingService.hpp"
#include "App/UI/WidgetInputService.hpp"
#include "App/UI/WidgetSpawningService.hpp"
#include "App/World/OpenWorldTracker.hpp"
#include "Core/Foundation/LocaleProvider.hpp"
#include "Core/Foundation/RuntimeProvider.hpp"
#if defined(_WIN32) || defined(_WIN64)
#include "Support/MinHook/MinHookProvider.hpp"
#endif
#include "Support/RED4ext/RED4extProvider.hpp"
#include "Support/RedLib/RedLibProvider.hpp"
#include "Support/Spdlog/SpdlogProvider.hpp"

App::Application::Application(HMODULE aHandle, const RED4ext::v1::Sdk* aSdk)
{
    Register<Core::LocaleProvider>();
#if defined(_WIN32) || defined(_WIN64)
    Register<Core::RuntimeProvider>(aHandle)
        ->SetBaseImagePathDepth(2);

    Register<Support::MinHookProvider>();
    Register<Support::SpdlogProvider>()
        ->AppendTimestampToLogName()
        ->CreateRecentLogSymlink();
    Register<Support::RED4extProvider>(aHandle, aSdk)
        ->EnableAddressLibrary()
        ->RegisterScripts(Env::ScriptsDir());
#else
    // macOS: the game root is three levels above the executable (Cyberpunk2077.app/Contents/MacOS/Cyberpunk2077).
    // Hooks go through RED4ext's native hook engine and addresses through the verified address database (see
    // macos/PORT_NOTES.md); there is no MinHook.
    Register<Core::RuntimeProvider>(aHandle)
        ->SetBaseImagePathDepth(3);

    Register<Support::SpdlogProvider>()
        ->AppendTimestampToLogName()
        ->CreateRecentLogSymlink();
    Register<Support::RED4extProvider>(aHandle, aSdk)
        ->EnableHooking()
        ->EnableAddressLibrary()
        ->RegisterScripts(Env::ScriptsDir());
#endif
    Register<Support::RedLibProvider>();

    Register<App::ScriptingService>(Env::PersistentDir());
    Register<App::LocalizationService>();
    Register<App::PersistencyService>();
    Register<App::ResourcePathRegistry>(Env::KnownHashesPath());
    Register<App::QuestPhaseRegistry>();
    Register<App::OpenWorldTracker>();
    Register<App::WidgetBuildingService>();
    Register<App::WidgetSpawningService>();
    Register<App::WidgetInputService>();
}

void App::Application::OnStarting()
{
    LogInfo("{} {} is initializing...", Project::Name, Project::Version.to_string());

    Migration::CleanUp(Env::LegacyScriptsDir());
}

void App::Application::OnStarted()
{
    LogInfo("{} is initialized.", Project::Name);
}
