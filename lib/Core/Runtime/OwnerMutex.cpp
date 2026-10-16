#include "OwnerMutex.hpp"

Core::OwnerMutex::OwnerMutex(std::string_view aName)
    : m_aname(aName)
    , m_mutex(nullptr)
{
}

Core::OwnerMutex::OwnerMutex(std::wstring_view aName)
    : m_wname(aName)
    , m_mutex(nullptr)
{
}

Core::OwnerMutex::~OwnerMutex()
{
    Release();
}

bool Core::OwnerMutex::Obtain()
{
#if !defined(_WIN32) && !defined(_WIN64)
    // macOS: the named mutex only guards against a second copy loaded as an ASI, which does not exist on macOS (the
    // RED4ext loader loads each plugin once). No lock file is created.
    m_mutex = reinterpret_cast<HANDLE>(1);
    return true;
#else
    const auto mutex = !m_wname.empty()
        ? CreateMutexW(NULL, TRUE, m_wname.data())
        : CreateMutexA(NULL, TRUE, m_aname.data());

    if (!mutex)
        return false;

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        ReleaseMutex(mutex);
        return false;
    }

    m_mutex = mutex;

    return true;
#endif
}

bool Core::OwnerMutex::Release()
{
    if (!m_mutex)
        return false;

#if defined(_WIN32) || defined(_WIN64)
    ReleaseMutex(m_mutex);
#endif
    m_mutex = nullptr;

    return true;
}

bool Core::OwnerMutex::IsOwner()
{
    return m_mutex;
}
