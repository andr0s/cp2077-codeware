#include "WidgetBuildingService.hpp"

void App::WidgetBuildingService::OnBootstrap()
{
#if defined(__APPLE__)
    // Off on macOS for now: the game crashed in its ink spawn jobs with Codeware's UI hooks in place, and none of the
    // mods in use (BT-7274, REDCheatMenu2) needs them.
    LogInfo("[macOS] Widget building hooks are off.");
#else
    HookAfter<Raw::inkWidget::ScriptReparent>(&AfterReparentFromScript).OrThrow();
    HookAfter<Raw::inkCompoundWidget::ScriptAddChild>(&AfterAddChildFromScript).OrThrow();
    Hook<Raw::inkLogicController::OnInitialize>(&OnInitializeController).OrThrow();
#endif
}

void App::WidgetBuildingService::OnShutdown()
{
    Unhook<Raw::inkWidget::ScriptReparent>();
    Unhook<Raw::inkCompoundWidget::ScriptAddChild>();
    Unhook<Raw::inkLogicController::OnInitialize>();
}

void App::WidgetBuildingService::AfterReparentFromScript(Red::inkWidget* aWidget, Red::CStackFrame*, void*, void*)
{
    if (!aWidget->layerProxy)
    {
        AttachWidgetTree(Red::AsHandle(aWidget));
    }
}

void App::WidgetBuildingService::AfterAddChildFromScript(Red::inkCompoundWidget* aParent, Red::CStackFrame*, void*, void*)
{
    if (aParent->layerProxy)
    {
        Red::Handle<Red::inkWidget> widget;
        {
            std::shared_lock _(aParent->children->lock);
            widget = *(aParent->children->children.End() - 1);
        }

        AttachWidgetTree(widget);
    }
}

void App::WidgetBuildingService::OnInitializeController(Red::inkLogicController* aController)
{
    if (!IsLegacyController(aController))
    {
        Raw::inkLogicController::OnInitialize(aController);
    }
}

bool App::WidgetBuildingService::AttachWidgetTree(const Red::Handle<Red::inkWidget>& aWidget)
{
    auto& layerProxy = aWidget->layerProxy;

    if (layerProxy)
        return true;

#if defined(__APPLE__)
    layerProxy = Raw::inkWidget::GetLayer(aWidget);
#else
    Raw::inkWidget::GetLayer(aWidget, layerProxy);
#endif

    if (!layerProxy)
        return false;

    auto layer = layerProxy->layer.Lock();

    if (!layer)
        return false;

    Raw::inkWidget::SetLayer(aWidget, layerProxy);
    Raw::inkLayer::AttachWidgetTree(layer, aWidget, layerProxy);

    return true;
}

bool App::WidgetBuildingService::AttachController(const Red::Handle<Red::inkWidget>& aWidget,
                                                  const Red::Handle<Red::inkLogicController>& aController)
{
    if (aController->layerProxy.instance)
        return true;

    aController->widget = aWidget;
    aController->layerProxy = aWidget->layerProxy;

    auto& layerProxy = aWidget->layerProxy;

    if (!layerProxy)
        return false;

    auto layer = layerProxy->layer.Lock();

    if (!layer)
        return false;

    Raw::inkLayer::RegisterListener(layer, aController);

    aController->OnInitialize();

    return true;
}

bool App::WidgetBuildingService::IsLegacyController(Red::inkLogicController* aController)
{
    return aController->nativeType
           && aController->nativeType->propsByName.Get("detachedWidget")
           && aController->nativeType->propsByName.Get("gameController");
}
