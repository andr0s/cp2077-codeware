#include "CallbackSystemController.hpp"

Core::Map<Red::CName, Red::CName> App::CallbackSystemController::GetMappings()
{
    return {};
}

bool App::CallbackSystemController::ActivateEvent(Red::CName aEvent)
{
    if (!m_activeEvents.contains(aEvent))
    {
        m_activeEvents.insert(aEvent);
        OnActivateEvent(aEvent);

        if (!m_initialized)
        {
#if defined(__APPLE__)
            // macOS: the hooks of an event whose game function is not located are refused; the event never fires.
            if (!OnActivateHook())
            {
                LogWarning("[macOS] Callback event \"{}\" is not available on macOS.", aEvent.ToString());
            }
#else
            OnActivateHook();
#endif
            m_initialized = true;
        }
    }

    return true;
}

bool App::CallbackSystemController::DeactivateEvent(Red::CName aEvent)
{
    if (m_activeEvents.contains(aEvent))
    {
        m_activeEvents.erase(aEvent);
        OnDeactivateEvent(aEvent);

        if (m_initialized && m_activeEvents.empty())
        {
            OnDeactivateHook();
            m_initialized = false;
        }
    }

    return true;
}

bool App::CallbackSystemController::IsActiveEvent(Red::CName aEvent)
{
    return m_activeEvents.contains(aEvent);
}
