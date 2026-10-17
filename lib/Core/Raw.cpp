#include "Raw.hpp"
#include "Core/Facades/Log.hpp"

#if !(defined(_WIN32) || defined(_WIN64))
void Core::Detail::ReportUnavailableFunction(const char* aSignature)
{
    Core::Log::Error("[macOS] Skipped a call to a game function that is not available on macOS: {}", aSignature);
}
#endif
