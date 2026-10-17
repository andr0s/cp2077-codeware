#pragma once

#if defined(__APPLE__)
#include "Core/Facades/Log.hpp"

namespace App
{
// macOS: a native whose game function or data layout is not available (or not verified) on macOS returns a default
// value instead. The first call of each feature logs one line.
inline void ReportUnavailableOnMacOS(std::once_flag& aReported, const char* aFeature)
{
    std::call_once(aReported, [aFeature] { Core::Log::Warning("[macOS] {} is not available on macOS.", aFeature); });
}
}
#endif
