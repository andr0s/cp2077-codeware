#include "WeatherSystemEx.hpp"
#include "Red/RuntimeScene.hpp"

bool App::WeatherSystemEx::SetWeather(Red::CName aWeather, Red::Optional<float> aBlendTime, Red::Optional<uint32_t> aPriority)
{
    auto* system = Raw::WeatherScriptInterface::System::Ref(this);

    if (!system)
        return false;

    if (!Raw::RuntimeSystemWeather::SetWeatherByName(system, aWeather, "", aBlendTime, 0, aPriority))
        return false;

    Raw::RuntimeSystemWeather::CycleWeather::Ref(system) = false;
    return true;
}

bool App::WeatherSystemEx::ResetWeather(Red::Optional<bool> aForceRestore, Red::Optional<float> aBlendTime)
{
    auto* system = Raw::WeatherScriptInterface::System::Ref(this);

    if (!system)
        return false;

    Raw::RuntimeSystemWeather::CycleWeather::Ref(system) = true;

    if (aForceRestore)
    {
#if defined(__APPLE__)
        // macOS: SetWeatherByIndex returns void; with index 0 it always applies the state.
        Raw::RuntimeSystemWeather::SetWeatherByIndex(system, 0, "", aBlendTime, 0, 0);
#else
        if (!Raw::RuntimeSystemWeather::SetWeatherByIndex(system, 0, "", aBlendTime, 0.0, 0))
            return false;
#endif
    }

    return true;
}

Red::Handle<Red::worldEnvironmentDefinition> App::WeatherSystemEx::GetEnvironmentDefinition()
{
    auto* system = Raw::WeatherScriptInterface::System::Ref(this);

    if (!system)
        return {};

#if defined(__APPLE__)
    return Raw::RuntimeSystemWeather::GetEnvironmentDefinition(system);
#else
    Red::Handle<Red::worldEnvironmentDefinition> definition;
    Raw::RuntimeSystemWeather::GetEnvironmentDefinition(system, definition);

    return definition;
#endif
}

Red::Handle<Red::worldWeatherState> App::WeatherSystemEx::GetWeatherState()
{
    auto* system = Raw::WeatherScriptInterface::System::Ref(this);

    if (!system)
        return {};

#if defined(__APPLE__)
    return Raw::RuntimeSystemWeather::GetWeatherState(system);
#else
    Red::Handle<Red::worldWeatherState> state;
    Raw::RuntimeSystemWeather::GetWeatherState(system, state);

    return state;
#endif
}
