#include "ScriptingService.hpp"
#include "App/Depot/ResourceReference.hpp"
#include "App/Engine/CurveData.hpp"

namespace
{
constexpr auto ISerializableTypeName = Red::GetTypeName<Red::ISerializable>();
constexpr auto IScriptableTypeName = Red::GetTypeName<Red::IScriptable>();

constexpr auto ResourceWrapperName = Red::GetTypeName<App::ResourceWrapper>();
constexpr auto ResourceAsyncWrapperName = Red::GetTypeName<App::ResourceAsyncWrapper>();
constexpr auto ResourceScriptReferenceAlias = Red::CName(Red::redResourceReferenceScriptToken::ALIAS);
constexpr auto ResourceReferencePrefix = Red::GetTypePrefixStr<Red::ResourceReference>();
constexpr auto ResourceAsyncReferencePrefix = Red::GetTypePrefixStr<Red::ResourceAsyncReference>();

constexpr auto CurveDataWrapperFloatName = Red::GetTypeName<App::CurveDataWrapper<float>>();
constexpr auto CurveDataWrapperVector2Name = Red::GetTypeName<App::CurveDataWrapper<Red::Vector2>>();
constexpr auto CurveDataWrapperVector3Name = Red::GetTypeName<App::CurveDataWrapper<Red::Vector3>>();
constexpr auto CurveDataWrapperVector4Name = Red::GetTypeName<App::CurveDataWrapper<Red::Vector4>>();
constexpr auto CurveDataWrapperHDRColorName = Red::GetTypeName<App::CurveDataWrapper<Red::HDRColor>>();
constexpr auto CurveDataPrefix = Red::GetTypePrefixStr<Red::CurveData>();

constexpr auto inkWidgetLibraryResourceTypeName = Red::CName("inkWidgetLibraryResource");
constexpr auto gameuiBaseUIDataTypeName = Red::CName("gameuiBaseUIData");
constexpr auto scnChatterTypeName = Red::CName("scnChatter");
constexpr auto audioEventElementTypeName = Red::CName("audioAudioEventMetadataArrayElement");
constexpr auto entLODDefinitionTypeName = Red::CName("entLODDefinition");
}

App::ScriptingService::ScriptingService(const std::filesystem::path& aStateDir)
{
    s_stateDir = aStateDir;

    std::error_code error;
    if (!std::filesystem::exists(s_stateDir, error))
    {
        if (!std::filesystem::create_directories(s_stateDir, error))
        {
            LogError("Cannot create directory \"{}\" for persistent data: {}.", s_stateDir.string(), error.message());
        }
    }
}

void App::ScriptingService::OnBootstrap()
{
#if defined(__APPLE__)
    // macOS: a hook whose address is not verified is not installed, and the plugin keeps loading (fail closed, one
    // log line each). The validator hooks are what let Codeware's script imports pass the game's script validation,
    // so their absence is an error. ScriptValidator::CompareTypeName is inlined on macOS: its name fallback runs
    // after ValidateProperty and CompareType instead (see OnValidateProperty and OnValidateScriptType).
    if (!HookBefore<Raw::ScriptValidator::Validate>(&OnValidateScripts))
        LogError("[macOS] Script validation hook (ScriptValidator::Validate) is not available.");
    if (!Raw::ScriptValidator::CompareType || !Hook<Raw::ScriptValidator::ValidateProperty>(&OnValidateProperty))
        LogError("[macOS] Script property validation hook (ScriptValidator::ValidateProperty) is not available.");
    if (!HookAfter<Raw::ScriptBundle::Destruct>(&OnInitializeScripts))
        LogError("[macOS] Scriptable services are disabled: ScriptBundle destructor hook is not available.");
    if (!HookAfter<Raw::CGameFramework::InitializeGameInstance>(&OnInitializeGameInstance))
        LogWarning("[macOS] ScriptableService.OnInitialize is disabled: game instance hook is not available.");
    if (!HookAfter<Raw::ScriptOpCodes::Register>(&OnRegisterScriptOpCodes))
        LogWarning("[macOS] The new/cast opcode extensions are disabled: opcode registration hook is not available.");
    if (Raw::ScriptOpCodes::Handlers && Raw::ScriptOpCodes::Handlers[Raw::ScriptOpCodes::New])
        OnRegisterScriptOpCodes(); // The game registered its opcodes before this plugin was loaded.
    if (!HookAfter<Raw::CClass::CreateInstance>(&OnCreateInstance))
        LogWarning("[macOS] OnConstruct callbacks are disabled: CClass::CreateInstance hook is not available.");
    if (!Hook<Raw::GetScriptGameInstance>(&GetScriptGameInstance))
        LogWarning("[macOS] GetGameInstance() override is disabled: its native is not available.");
#else
    HookAfter<Raw::ScriptBundle::Destruct>(&OnInitializeScripts).OrThrow();
    HookAfter<Raw::CGameFramework::InitializeGameInstance>(&OnInitializeGameInstance).OrThrow();
    HookBefore<Raw::ScriptValidator::Validate>(&OnValidateScripts).OrThrow();
    Hook<Raw::ScriptValidator::ValidateProperty>(&OnValidateProperty).OrThrow();
    HookAfter<Raw::ScriptValidator::CompareTypeName>(&OnValidateTypeName).OrThrow();
    HookAfter<Raw::ScriptOpCodes::Register>(&OnRegisterScriptOpCodes).OrThrow();
    HookAfter<Raw::CClass::CreateInstance>(&OnCreateInstance).OrThrow();
    Hook<Raw::GetScriptGameInstance>(&GetScriptGameInstance).OrThrow();
#endif
}

void App::ScriptingService::OnInitializeScripts()
{
    if (Red::GetGlobalFunction("InitializeScripts;"))
    {
        if (!Red::CGameEngine::Get()->scriptsLoaded)
        {
            ScriptableServiceContainer::Build(s_stateDir)->OnInitializeScripts();
        }
        else
        {
            ScriptableServiceContainer::Get()->OnInitializeScripts();
        }
    }
}

void App::ScriptingService::OnInitializeGameInstance()
{
#if defined(__APPLE__)
    if (!ScriptableServiceContainer::Get())
        return; // macOS: the container is built by the ScriptBundle hook, which may be unavailable
#endif
    ScriptableServiceContainer::Get()->OnInitializeGameInstance();
}

void App::ScriptingService::OnValidateScripts(void* aValidator, Red::ScriptBundle* aBundle, void* aReport)
{
    Red::GetClass<gameuiBaseUIDataTypeName>()->parent = Red::GetClass<ISerializableTypeName>();
    Red::GetClass<scnChatterTypeName>()->parent = Red::GetClass<ISerializableTypeName>();

    for (auto& classDef : aBundle->classes)
    {
        if (classDef->flags.isNative)
        {
            if (classDef->name == ISerializableTypeName)
            {
                classDef->flags.isStruct = true;
                classDef->parent = nullptr;
            }
            else if (classDef->name == audioEventElementTypeName || classDef->name == entLODDefinitionTypeName)
            {
                classDef->flags.isStruct = true;
                classDef->parent = reinterpret_cast<Red::ScriptClass*>(*aBundle->definitionsByName.Get(ISerializableTypeName));
            }
            else if (!classDef->flags.isStruct)
            {
                auto parentDef = classDef->parent;
                while (parentDef)
                {
                    if (parentDef->name == IScriptableTypeName)
                    {
                        break;
                    }
                    if (parentDef->name == ISerializableTypeName)
                    {
                        classDef->flags.isStruct = true;
                        break;
                    }
                    parentDef = parentDef->parent;
                }
            }

            if (classDef->flags.isStruct && classDef->name != inkWidgetLibraryResourceTypeName)
            {
                if (auto nativeClass = Red::GetScriptClass(classDef->name))
                {
                    classDef->flags.isAbstract = nativeClass->flags.isAbstract;

                    if (!classDef->parent && nativeClass->parent)
                    {
                        auto nativeParent = nativeClass->parent;
                        auto parentDef = aBundle->definitionsByName.Get(nativeParent->name);

                        if (!parentDef)
                        {
                            if (auto parentAlias = Red::GetScriptAlias(nativeParent->name))
                            {
                                parentDef = aBundle->definitionsByName.Get(parentAlias);
                            }
                        }

                        classDef->parent = reinterpret_cast<Red::ScriptClass*>(*parentDef);
                    }
                }
            }
        }
    }
}

bool App::ScriptingService::OnValidateProperty(void* aValidator, Red::ScriptProperty* aScriptProp,
                                               Red::CProperty* aNativeProp)
{
    auto* scriptType = aScriptProp->type;
    auto* nativeType = aNativeProp->type;

    if (scriptType->metaType == Red::EScriptType::DynArray ||
        scriptType->metaType == Red::EScriptType::StaticArray)
    {
        return OnValidateScriptType(nativeType, scriptType);
    }

    if (nativeType->GetType() == Red::ERTTIType::BitField && scriptType->metaType == Red::EScriptType::Primitive)
    {
        return true;
    }

#if defined(__APPLE__)
    return Raw::ScriptValidator::ValidateProperty(aValidator, aScriptProp, aNativeProp)
        || OnValidateTypeNameFallback(nativeType, scriptType);
#else
    return Raw::ScriptValidator::ValidateProperty(aValidator, aScriptProp, aNativeProp);
#endif
}

bool App::ScriptingService::OnValidateScriptType(Red::CBaseRTTIType* aNativeType, Red::ScriptType* aScriptType)
{
    if (aScriptType->metaType == Red::EScriptType::DynArray)
    {
        if (aNativeType->GetType() == Red::ERTTIType::Array)
        {
            auto nativeInnerType = reinterpret_cast<Red::CRTTIBaseArrayType*>(aNativeType)->innerType;
            auto scriptInnerType = reinterpret_cast<Red::ScriptType*>(aScriptType->innerType);

            return OnValidateScriptType(nativeInnerType, scriptInnerType);
        }
        return false;
    }

    if (aScriptType->metaType == Red::EScriptType::StaticArray)
    {
        if (aNativeType->GetType() == Red::ERTTIType::StaticArray ||
            aNativeType->GetType() == Red::ERTTIType::NativeArray ||
            aNativeType->GetType() == Red::ERTTIType::FixedArray)
        {
            auto nativeInnerType = reinterpret_cast<Red::CRTTIBaseArrayType*>(aNativeType)->innerType;
            auto scriptInnerType = reinterpret_cast<Red::ScriptType*>(aScriptType->innerType);

            return OnValidateScriptType(nativeInnerType, scriptInnerType);
        }
        return false;
    }

#if defined(__APPLE__)
    return Raw::ScriptValidator::CompareType(aNativeType, aScriptType)
        || OnValidateTypeNameFallback(aNativeType, aScriptType);
#else
    return Raw::ScriptValidator::CompareType(aNativeType, aScriptType);
#endif
}

#if defined(__APPLE__)
bool App::ScriptingService::OnValidateTypeNameFallback(Red::CBaseRTTIType* aNativeType, Red::ScriptType* aScriptType)
{
    // macOS: ValidateProperty (0x1021FCFB8) and CompareType (0x1021FD144) compare the script type name with the native
    // type name inline (and through CRTTISystem::ConvertScriptToNativeName). These are the names they compare for a
    // primitive and for a class or struct type; the Windows CompareTypeName hook gets the same pair.
    Red::CName scriptTypeName;
    if (aScriptType->metaType == Red::EScriptType::Primitive)
    {
        scriptTypeName = aScriptType->name;
    }
    else if (aScriptType->metaType == Red::EScriptType::Class)
    {
        scriptTypeName = aScriptType->innerType ? aScriptType->innerType->name : aScriptType->name;
    }
    else
    {
        return false;
    }

    bool valid = false;
    OnValidateTypeName(valid, scriptTypeName, aNativeType->GetName());
    return valid;
}
#endif

void App::ScriptingService::OnValidateTypeName(bool& aValid, Red::CName aScriptTypeName, Red::CName aNativeTypeName)
{
    if (!aValid)
    {
        if (aScriptTypeName == ResourceWrapperName)
        {
            aValid = strncmp(ResourceReferencePrefix.data(), aNativeTypeName.ToString(),
                             ResourceReferencePrefix.size() - 1) == 0;
        }
        else if (aScriptTypeName == ResourceAsyncWrapperName || aScriptTypeName == ResourceScriptReferenceAlias)
        {
            aValid = strncmp(ResourceAsyncReferencePrefix.data(), aNativeTypeName.ToString(),
                             ResourceAsyncReferencePrefix.size() - 1) == 0;
        }
        else if (aScriptTypeName == CurveDataWrapperFloatName || aScriptTypeName == CurveDataWrapperVector2Name ||
                 aScriptTypeName == CurveDataWrapperVector3Name || aScriptTypeName == CurveDataWrapperVector4Name ||
                 aScriptTypeName == CurveDataWrapperHDRColorName)
        {
            aValid = strncmp(CurveDataPrefix.data(), aNativeTypeName.ToString(), CurveDataPrefix.size() - 1) == 0;
        }
    }
}

void App::ScriptingService::OnRegisterScriptOpCodes()
{
    Raw::ScriptOpCodes::Handlers[Raw::ScriptOpCodes::New] = &ScriptOpCodeNew;
    Raw::ScriptOpCodes::Handlers[Raw::ScriptOpCodes::Cast] = &ScriptOpCodeCast;
}

void App::ScriptingService::ScriptOpCodeNew(Red::IScriptable*, Red::CStackFrame* aFrame, void* aRet,
                                            Red::CBaseRTTIType*)
{
    auto instanceType = *reinterpret_cast<Red::CClass**>(aFrame->code);
    aFrame->code += sizeof(Red::CClass*);

    if (aRet)
    {
        *reinterpret_cast<Red::Handle<Red::ISerializable>*>(aRet) = Red::MakeScriptedHandle(instanceType);
    }
}

void App::ScriptingService::ScriptOpCodeCast(Red::IScriptable*, Red::CStackFrame* aFrame, void* aLhs,
                                             Red::CBaseRTTIType*)
{
    auto lhsType = *reinterpret_cast<Red::CClass**>(aFrame->code);
    aFrame->code += sizeof(Red::CClass*);

    auto isWeak = *reinterpret_cast<bool*>(aFrame->code);
    aFrame->code += sizeof(bool);

    if (isWeak)
    {
        Red::WeakHandle<Red::ISerializable> rhsWeak;
        Red::GetParameter(aFrame, &rhsWeak);

        if (aLhs)
        {
            auto rhs = rhsWeak.Lock();
            if (rhs && !rhs->GetType()->IsA(lhsType))
            {
                rhs.Reset();
            }

            *reinterpret_cast<Red::WeakHandle<Red::ISerializable>*>(aLhs) = rhs;
        }
    }
    else
    {
        Red::Handle<Red::ISerializable> rhs;
        Red::GetParameter(aFrame, &rhs);

        if (aLhs)
        {
            if (rhs && !rhs->GetType()->IsA(lhsType))
            {
                rhs.Reset();
            }

            *reinterpret_cast<Red::Handle<Red::ISerializable>*>(aLhs) = rhs;
        }
    }
}

void App::ScriptingService::OnCreateInstance(Red::IScriptable*& aInstance, Red::CClass* aClass, uint64_t, bool)
{
    if (aInstance && aClass->flags.isScriptedClass)
    {
        Red::CallVirtual(aInstance, aClass, "OnConstruct");
    }
}

void App::ScriptingService::GetScriptGameInstance(Red::IScriptable*, Red::CStackFrame* aFrame,
                                                  Red::ScriptGameInstance* aRet, Red::CBaseRTTIType*)
{
    ++aFrame->code;

    if (aRet)
    {
        *aRet = Red::ScriptGameInstance{};
    }
}
