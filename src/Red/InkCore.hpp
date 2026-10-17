#pragma once

namespace Red
{
struct EventManager {};
}

namespace Raw::inkLayer
{
constexpr auto RegisterListener = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkLayer_RegisterListener,
    /* type = */ void (*)(Red::inkLayer* aLayer, const Red::Handle<Red::IScriptable>& aListener)>();

constexpr auto AttachWidgetTree = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkLayer_AttachWidgetTree,
#if defined(__APPLE__)
    /* type = */ void (*)(Red::inkLayer* aLayer, // macOS: returns void
#else
    /* type = */ uint64_t (*)(Red::inkLayer* aLayer,
#endif
                              const Red::Handle<Red::inkWidget>& aWidget,
                              const Red::WeakHandle<Red::inkLayerProxy>& aProxy)>();
}

namespace Raw::inkWidget
{
using EventManager = Core::OffsetPtr<0x50, Red::EventManager>;

#if defined(__APPLE__)
// macOS: the proxy handle is returned through x8 (0x1049ADE28); it is also cached in widget->layerProxy.
constexpr auto GetLayer = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_GetLayer,
    /* type = */ Red::Handle<Red::inkLayerProxy> (*)(Red::inkWidget* aWidget)>();
#else
constexpr auto GetLayer = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_GetLayer,
    /* type = */ void (*)(Red::inkWidget* aWidget, Red::Handle<Red::inkLayerProxy>& aProxy)>();
#endif

constexpr auto SetLayer = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_SetLayer,
    /* type = */ void (*)(Red::inkWidget* aWidget, Red::Handle<Red::inkLayerProxy>& aProxy)>();

constexpr auto ScriptReparent = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_ScriptReparent,
    /* type = */ void (*)(Red::inkWidget* aWidget, Red::CStackFrame* aFrame, void*, void*)>();

constexpr auto TriggerEvent = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_TriggerEvent,
    /* type = */ bool (*)(void* aManager, Red::CName aName, Red::Handle<Red::Event> aObject)>();
}

namespace Raw::inkCompoundWidget
{
constexpr auto ScriptAddChild = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkWidget_ScriptAddChild,
    /* type = */ void (*)(Red::inkCompoundWidget* aWidget, Red::CStackFrame* aFrame, void*, void*)>();
}

#if defined(__APPLE__)
namespace Raw::inkWorldLayer
{
// macOS: clang places the members after inkLayer's tail padding, so the components array is at +0x168 (SDK: +0x170).
// Proof: inkWorldLayer::AddComponent 0x104919988 locks +0x160 and appends to +0x168.
using Components = Core::OffsetPtr<0x168, Red::DynArray<Red::SharedPtr<Red::ink::IWidgetComponentWrapper>>>;
}

namespace Raw::IWidgetComponentWrapper
{
// macOS: GetGameController is vtable slot +0x90 (0x1009FA654, handle through x8); the SDK declaration lands on +0x98.
constexpr auto GetGameController = Core::RawVFunc<
    /* addr = */ 0x88,
    /* type = */ Red::Handle<Red::ink::IWidgetController> (*)(Red::ink::IWidgetComponentWrapper* aWrapper)>();
}
#endif

namespace Raw::inkLogicController
{
constexpr auto OnInitialize = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkLogicController_OnInitialize,
    /* type = */ void (*)(Red::inkLogicController* aController)>();

constexpr auto OnUninitialize = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkLogicController_OnUninitialize,
    /* type = */ void (*)(Red::inkLogicController* aController)>();

constexpr auto OnArrangeChildrenComplete = Core::RawFunc<
    /* addr = */ Red::AddressLib::InkLogicController_OnArrangeChildrenComplete,
    /* type = */ void (*)(Red::inkLogicController* aController)>();
}
