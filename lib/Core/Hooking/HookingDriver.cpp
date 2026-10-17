#include "HookingDriver.hpp"
#include "HookingAgent.hpp"
#include "Core/Facades/Log.hpp"

#include <cassert>

namespace
{
Core::HookingDriver* s_default;
}

void Core::HookingDriver::SetDefault(Core::HookingDriver& aDriver)
{
    s_default = &aDriver;

    HookingAgent::SetHookingDriver(aDriver);
}

Core::HookingDriver& Core::HookingDriver::GetDefault()
{
    assert(s_default);
    return *s_default;
}

#if !(defined(_WIN32) || defined(_WIN64))
void Core::Detail::ReportFailedHook(const std::string& aMessage)
{
    Core::Log::Error("[macOS] {} The feature that needs this hook is disabled.", aMessage);
}
#endif
