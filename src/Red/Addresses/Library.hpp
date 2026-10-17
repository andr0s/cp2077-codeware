#pragma once

// macOS: an address that is not in the verified macOS address database, and was not found with evidence for this port
// (macos/ADDRESSES.md, macos/addresses_proposed.json), is 0. The hash then never appears in the binary (RED4ext refuses
// a plugin that contains an unverified hash), it resolves to 0, hooks on it are refused, and the features that need it
// are disabled with one log line. See macos/PORT_NOTES.md.
#if defined(__APPLE__)
#define RED_ADDRESS_MACOS_UNAVAILABLE(aHash) 0
#else
#define RED_ADDRESS_MACOS_UNAVAILABLE(aHash) aHash
#endif

namespace Red::AddressLib
{
constexpr uint32_t Main = 240386859;
constexpr uint32_t GetScriptGameInstance = 1559305924;

constexpr uint32_t CBaseEngine_InitEngine = 3273923080;
constexpr uint32_t CBaseEngine_LoadGatheredResources = 3729789488;

constexpr uint32_t CBaseFunction_InternalExecute = RED_ADDRESS_MACOS_UNAVAILABLE(2744524784); // rtti::Function::InternalCallNative

constexpr uint32_t CBaseStack_ResolveContext = RED_ADDRESS_MACOS_UNAVAILABLE(1745688837);

constexpr uint32_t CGameFramework_InitializeGameInstance = 4007601841;

constexpr uint32_t CMesh_GetAppearance = 773462733;
constexpr uint32_t CMesh_FindAppearance = 3007126651;

constexpr uint32_t CommunitySystem_Update = RED_ADDRESS_MACOS_UNAVAILABLE(1559565515);

constexpr uint32_t EngineTime_Frequency = RED_ADDRESS_MACOS_UNAVAILABLE(3151104823);

constexpr uint32_t Entity_Attach = 4248638169;
constexpr uint32_t Entity_Detach = 2263681375;
constexpr uint32_t Entity_Dispose = 2515274237;
constexpr uint32_t Entity_Initialize = 3490519617;
constexpr uint32_t Entity_Assemble = RED_ADDRESS_MACOS_UNAVAILABLE(2182550867);
constexpr uint32_t Entity_Reassemble = 1560690857;
constexpr uint32_t Entity_Uninitialize = 3596356635;
constexpr uint32_t Entity_RequestComponents = RED_ADDRESS_MACOS_UNAVAILABLE(2296260874);
constexpr uint32_t Entity_ResolveRecordID = RED_ADDRESS_MACOS_UNAVAILABLE(525805301);

constexpr uint32_t EntityBuilder_ExtractComponentsJob = RED_ADDRESS_MACOS_UNAVAILABLE(489494088);
constexpr uint32_t EntityBuilder_ScheduleExtractComponentsJob = 437791594;

constexpr uint32_t EntitySpawner_SpawnEntity = RED_ADDRESS_MACOS_UNAVAILABLE(3068279668);

constexpr uint32_t EntitySpawnerToken_dtor = RED_ADDRESS_MACOS_UNAVAILABLE(552341017);
constexpr uint32_t EntitySpawnerToken_GetWaitHandle = RED_ADDRESS_MACOS_UNAVAILABLE(2091718968);

constexpr uint32_t InkLayer_RegisterListener = RED_ADDRESS_MACOS_UNAVAILABLE(4792499);
constexpr uint32_t InkLayer_AttachWidgetTree = RED_ADDRESS_MACOS_UNAVAILABLE(2050958960);

constexpr uint32_t InkLogicController_OnInitialize = RED_ADDRESS_MACOS_UNAVAILABLE(2671382832);
constexpr uint32_t InkLogicController_OnUninitialize = RED_ADDRESS_MACOS_UNAVAILABLE(1159862457);
constexpr uint32_t InkLogicController_OnArrangeChildrenComplete = RED_ADDRESS_MACOS_UNAVAILABLE(3589084925);

constexpr uint32_t InkSpawner_FinishAsyncSpawn = 2698985195;

constexpr uint32_t InkSystem_Instance = 2635337807;
constexpr uint32_t InkSystem_ProcessCharacterEvent = RED_ADDRESS_MACOS_UNAVAILABLE(4284882427);
constexpr uint32_t InkSystem_ProcessInputEvents = RED_ADDRESS_MACOS_UNAVAILABLE(673454961);

constexpr uint32_t InkWidget_GetLayer = RED_ADDRESS_MACOS_UNAVAILABLE(1667176558);
constexpr uint32_t InkWidget_SetLayer = RED_ADDRESS_MACOS_UNAVAILABLE(2516523266);
constexpr uint32_t InkWidget_ScriptReparent = RED_ADDRESS_MACOS_UNAVAILABLE(492509908);
constexpr uint32_t InkWidget_ScriptAddChild = RED_ADDRESS_MACOS_UNAVAILABLE(3558023194);
constexpr uint32_t InkWidget_TriggerEvent = RED_ADDRESS_MACOS_UNAVAILABLE(3357288308);

constexpr uint32_t InkWidgetLibrary_AsyncSpawnFromExternal = 1396063719;
constexpr uint32_t InkWidgetLibrary_AsyncSpawnFromLocal = 118698863;
constexpr uint32_t InkWidgetLibrary_SpawnFromExternal = 506278179;
constexpr uint32_t InkWidgetLibrary_SpawnFromLocal = 1158555307;

constexpr uint32_t IComponent_Toggle = RED_ADDRESS_MACOS_UNAVAILABLE(3370191745);

constexpr uint32_t IPlacedComponent_SetTransform = RED_ADDRESS_MACOS_UNAVAILABLE(1828854026);

constexpr uint32_t ISerializable_Clone = RED_ADDRESS_MACOS_UNAVAILABLE(2601784524);

constexpr uint32_t IScriptable_CheckClass = RED_ADDRESS_MACOS_UNAVAILABLE(2487884020);
constexpr uint32_t IScriptable_CheckExactClass = RED_ADDRESS_MACOS_UNAVAILABLE(1797004238);

constexpr uint32_t JobHandle_Wait = 1576079097;

constexpr uint32_t Localization_LoadOnScreens = 3550098299;

constexpr uint32_t MeshAppearance_LoadMaterialSetupAsync = 1419388740;
constexpr uint32_t MeshAppearance_ReleaseReference = RED_ADDRESS_MACOS_UNAVAILABLE(1679234269);

constexpr uint32_t MorphTargetManager_ApplyMorphTarget = RED_ADDRESS_MACOS_UNAVAILABLE(633871992);

constexpr uint32_t NodeRef_Create = RED_ADDRESS_MACOS_UNAVAILABLE(3491172781);

constexpr uint32_t ObjectSerializer_Prepare = RED_ADDRESS_MACOS_UNAVAILABLE(1944983789);
constexpr uint32_t ObjectSerializer_WriteToBuffer = RED_ADDRESS_MACOS_UNAVAILABLE(2646156567);
constexpr uint32_t ObjectSerializer_ReadFromStrean = RED_ADDRESS_MACOS_UNAVAILABLE(125247621);

constexpr uint32_t PersistentObject_InitializeState = RED_ADDRESS_MACOS_UNAVAILABLE(3442350580);

constexpr uint32_t PhysicsTraceResult_GetHitObject = RED_ADDRESS_MACOS_UNAVAILABLE(2394822845);

constexpr uint32_t PlayerSystem_OnPlayerSpawned = RED_ADDRESS_MACOS_UNAVAILABLE(2050111212); // cp::PlayerSystem::OnPlayerMainObjectSpawned

constexpr uint32_t PointOfInterestMappin_SetPhase = RED_ADDRESS_MACOS_UNAVAILABLE(803085170);

constexpr uint32_t QuestLoader_PhasePreloadCheck = RED_ADDRESS_MACOS_UNAVAILABLE(1567104722);

constexpr uint32_t QuestPhaseInstance_Initialize = RED_ADDRESS_MACOS_UNAVAILABLE(3423879019);
constexpr uint32_t QuestPhaseInstance_ExecuteGraph = RED_ADDRESS_MACOS_UNAVAILABLE(779107884);
constexpr uint32_t QuestPhaseInstance_ExecuteNode = RED_ADDRESS_MACOS_UNAVAILABLE(3227858325);

constexpr uint32_t QuestsSystem_CreateContext = RED_ADDRESS_MACOS_UNAVAILABLE(3144298192);

constexpr uint32_t RenderData_Release = RED_ADDRESS_MACOS_UNAVAILABLE(2996308699);

constexpr uint32_t RenderProxy_SetMaterialOverride = RED_ADDRESS_MACOS_UNAVAILABLE(429336746);
constexpr uint32_t RenderProxy_SetHighlightParams = RED_ADDRESS_MACOS_UNAVAILABLE(1093803822);
constexpr uint32_t RenderProxy_SetScanningState = RED_ADDRESS_MACOS_UNAVAILABLE(2838044016);
constexpr uint32_t RenderProxy_SetVisibility = RED_ADDRESS_MACOS_UNAVAILABLE(1790971865);
constexpr uint32_t RenderProxy_UpdateVisibility = RED_ADDRESS_MACOS_UNAVAILABLE(2124419179);

constexpr uint32_t ResourceBank_ForgetResource = RED_ADDRESS_MACOS_UNAVAILABLE(2288918343);

constexpr uint32_t ResourcePath_Create = 3998356057;

constexpr uint32_t ResourceSerializer_ExecutePostLoad = RED_ADDRESS_MACOS_UNAVAILABLE(1350901460);
constexpr uint32_t ResourceSerializer_SchedulePostLoadJobs = RED_ADDRESS_MACOS_UNAVAILABLE(3599579551);
constexpr uint32_t ResourceSerializer_ScheduleFinalizeJobs = RED_ADDRESS_MACOS_UNAVAILABLE(949631709);
constexpr uint32_t ResourceSerializer_OnDependenciesReady = 1185093671;
constexpr uint32_t ResourceSerializer_OnResourceReady = RED_ADDRESS_MACOS_UNAVAILABLE(1147149338);

constexpr uint32_t RuntimeEntityRegistry_RegisterEntity = RED_ADDRESS_MACOS_UNAVAILABLE(2840271332); // world::RuntimeEntityRegistry::RegisterEntity
constexpr uint32_t RuntimeEntityRegistry_UnregisterEntity = RED_ADDRESS_MACOS_UNAVAILABLE(3878623943);

constexpr uint32_t RuntimeScene_AttachEntity = RED_ADDRESS_MACOS_UNAVAILABLE(3800374702);
constexpr uint32_t RuntimeScene_DetachEntity = RED_ADDRESS_MACOS_UNAVAILABLE(1732384813);

constexpr uint32_t RuntimeSystemEntity_AttachEntity = RED_ADDRESS_MACOS_UNAVAILABLE(1576214475);
constexpr uint32_t RuntimeSystemEntity_DetachEntity = RED_ADDRESS_MACOS_UNAVAILABLE(1737563910);

constexpr uint32_t RuntimeSystemWeather_GetEnvironmentDefinition = RED_ADDRESS_MACOS_UNAVAILABLE(1471948702);
constexpr uint32_t RuntimeSystemWeather_GetWeatherState = RED_ADDRESS_MACOS_UNAVAILABLE(2679119876);
constexpr uint32_t RuntimeSystemWeather_SetWeatherByName = RED_ADDRESS_MACOS_UNAVAILABLE(2334794340);
constexpr uint32_t RuntimeSystemWeather_SetWeatherByIndex = RED_ADDRESS_MACOS_UNAVAILABLE(1821516328);

constexpr uint32_t ScriptBundle_dtor = 3993832650;

constexpr uint32_t ScriptOpCodes_Handlers = 961751128;
constexpr uint32_t ScriptOpCodes_Handlers_New = RED_ADDRESS_MACOS_UNAVAILABLE(2669615409);
constexpr uint32_t ScriptOpCodes_Handlers_Cast = RED_ADDRESS_MACOS_UNAVAILABLE(2439126103);
constexpr uint32_t ScriptOpCodes_Register = 2070546748;

constexpr uint32_t ScriptValidator_CompareType = 4224523662;
constexpr uint32_t ScriptValidator_CompareTypeName = RED_ADDRESS_MACOS_UNAVAILABLE(1156189659);
constexpr uint32_t ScriptValidator_Validate = 898639042;
constexpr uint32_t ScriptValidator_ValidateProperty = 560604283;

constexpr uint32_t SoundBankManager_ReadSoundBanksJson = RED_ADDRESS_MACOS_UNAVAILABLE(3961524856);

constexpr uint32_t TagSystem_AssignTag = RED_ADDRESS_MACOS_UNAVAILABLE(1239092911);
constexpr uint32_t TagSystem_UnassignTag = RED_ADDRESS_MACOS_UNAVAILABLE(1875775378);

constexpr uint32_t VehicleGarageComponent_LoadSummonSetup = RED_ADDRESS_MACOS_UNAVAILABLE(1159928678);

constexpr uint32_t VehicleController_ToggleAuxLights = RED_ADDRESS_MACOS_UNAVAILABLE(1883248814);

constexpr uint32_t VehicleSystem_ToggleGarageVehicle = RED_ADDRESS_MACOS_UNAVAILABLE(3027836941);

constexpr uint32_t VisionModeSystem_SetBraindanceMode = RED_ADDRESS_MACOS_UNAVAILABLE(1070077985);

constexpr uint32_t VisualController_LoadDependencies = RED_ADDRESS_MACOS_UNAVAILABLE(2912168594);

constexpr uint32_t WorkspotSystem_GetWorkspotInstance = RED_ADDRESS_MACOS_UNAVAILABLE(2234523611);
}
