#pragma once

#include "Red/GameEngine.hpp"

namespace App
{
struct EngineTimeEx : Red::EngineTime
{
    static uint64_t GetFrequency()
    {
        // The global resolves to null when its address is not in the database.
        return Raw::EngineTime::Frequency ? static_cast<uint64_t>(*Raw::EngineTime::Frequency) : 0;
    }

    static Red::EngineTime FromTicks(uint64_t aTicks)
    {
        return {aTicks};
    }

    static uint64_t ToTicks(Red::EngineTime aTime)
    {
        return aTime.ticks;
    }
};
}

RTTI_EXPAND_CLASS(Red::EngineTime, App::EngineTimeEx, {
    RTTI_METHOD(GetFrequency);
    RTTI_METHOD(FromTicks);
    RTTI_METHOD(ToTicks);
});
