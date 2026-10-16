#include "RED4extProvider.hpp"

Support::RED4extProvider::RED4extProvider(RED4ext::v1::PluginHandle aPlugin, const RED4ext::v1::Sdk* aSdk) noexcept
    : m_plugin(aPlugin)
    , m_sdk(aSdk)
    , m_enableLogging(false)
    , m_enableHooking(false)
    , m_enableAddressLibrary(false)
{
}

void Support::RED4extProvider::OnInitialize()
{
    if (m_enableLogging)
    {
        LoggingDriver::SetDefault(*this);
    }

    if (m_enableHooking)
    {
        HookingDriver::SetDefault(*this);
    }

    if (m_enableAddressLibrary)
    {
        AddressResolver::SetDefault(*this);
    }
}

void Support::RED4extProvider::LogInfo(const std::string_view& aMessage)
{
    m_sdk->logger->Info(m_plugin, aMessage.data());
}

void Support::RED4extProvider::LogWarning(const std::string_view& aMessage)
{
    m_sdk->logger->Warn(m_plugin, aMessage.data());
}

void Support::RED4extProvider::LogError(const std::string_view& aMessage)
{
    m_sdk->logger->Error(m_plugin, aMessage.data());
}

void Support::RED4extProvider::LogDebug(const std::string_view& aMessage)
{
    m_sdk->logger->Debug(m_plugin, aMessage.data());
}

void Support::RED4extProvider::LogFlush()
{
}

bool Support::RED4extProvider::HookAttach(uintptr_t aAddress, void* aCallback)
{
#if defined(__APPLE__)
    // macOS: an address that did not resolve (not in the verified database, or disabled on macOS) is never hooked.
    if (!aAddress)
        return false;
#endif
    return m_sdk->hooking->Attach(m_plugin, reinterpret_cast<void*>(aAddress), aCallback, nullptr);
}

bool Support::RED4extProvider::HookAttach(uintptr_t aAddress, void* aCallback, void** aOriginal)
{
#if defined(__APPLE__)
    if (!aAddress)
        return false;
#endif
    return m_sdk->hooking->Attach(m_plugin, reinterpret_cast<void*>(aAddress), aCallback, aOriginal);
}

bool Support::RED4extProvider::HookDetach(uintptr_t aAddress)
{
    return m_sdk->hooking->Detach(m_plugin, reinterpret_cast<void*>(aAddress));
}

uintptr_t Support::RED4extProvider::ResolveAddress(uint32_t aAddressID)
{
#if defined(__APPLE__)
    // macOS: hash 0 marks an address that is not available on macOS (src/Red/Addresses/Library.hpp); the feature
    // using it is disabled, so do not ask the database (and do not log a missing hash for it).
    if (!aAddressID)
        return 0;
#endif
    return RED4ext::UniversalRelocBase::Resolve(aAddressID);
}
