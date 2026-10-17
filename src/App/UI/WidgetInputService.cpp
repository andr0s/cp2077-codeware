#include "WidgetInputService.hpp"
#include "App/UI/inkKeyInputEvent.hpp"
#include "Red/InkCore.hpp"

#if defined(__APPLE__)
#include <CoreGraphics/CGEventSource.h>
#endif

namespace
{
constexpr auto RawInputEventName = Red::CName("OnInputKey");
}

void App::WidgetInputService::OnBootstrap()
{
#if defined(__APPLE__)
    if (!HookBefore<Raw::inkSystem::ProcessCharacterEvent>(&OnCharacterInput))
        LogWarning("[macOS] Widget key input events (inkKeyInputEvent) are not available on macOS.");
#else
    HookBefore<Raw::inkSystem::ProcessCharacterEvent>(&OnCharacterInput).OrThrow();
#endif
}

void App::WidgetInputService::OnShutdown()
{
    Unhook<Raw::inkSystem::ProcessCharacterEvent>();
}

void App::WidgetInputService::OnCharacterInput(Red::InkSystem* aSystem, Red::EInputKey aKey, Red::EInputAction aAction)
{
    if (aAction == Red::EInputAction::IACT_Press || aAction == Red::EInputAction::IACT_Release)
    {
        if (aSystem->inputWidget)
        {
            auto event = Red::MakeHandle<inkKeyInputEvent>(aSystem->keyboardState, aKey, aAction, ToCharacter(aKey));
            auto manager = Raw::inkWidget::EventManager(aSystem->inputWidget.instance);
            Raw::inkWidget::TriggerEvent(manager, RawInputEventName, event);
        }
    }
}

std::string App::WidgetInputService::ToCharacter(Red::EInputKey aKey)
{
    if (!IsCharacterInput(aKey))
        return {};

#if defined(__APPLE__)
    // macOS: there is no GetKeyboardState/ToUnicode. EInputKey values are Windows virtual-key codes; map them with the
    // US ANSI layout, using the current Shift and Caps Lock state (CGEventSourceFlagsState is safe on any thread).
    const auto flags = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
    const bool shift = (flags & kCGEventFlagMaskShift) != 0;
    const bool capsLock = (flags & kCGEventFlagMaskAlphaShift) != 0;
    const auto code = static_cast<uint32_t>(aKey);

    if (aKey == Red::EInputKey::IK_Space)
        return " ";

    if (aKey >= Red::EInputKey::IK_A && aKey <= Red::EInputKey::IK_Z)
    {
        const char base = (shift != capsLock) ? 'A' : 'a';
        return {static_cast<char>(base + (code - static_cast<uint32_t>(Red::EInputKey::IK_A)))};
    }

    if (aKey >= Red::EInputKey::IK_0 && aKey <= Red::EInputKey::IK_9)
    {
        constexpr const char* shifted = ")!@#$%^&*(";
        const auto index = code - static_cast<uint32_t>(Red::EInputKey::IK_0);
        return {shift ? shifted[index] : static_cast<char>('0' + index)};
    }

    if (aKey >= Red::EInputKey::IK_NumPad0 && aKey <= Red::EInputKey::IK_NumPad9)
        return {static_cast<char>('0' + (code - static_cast<uint32_t>(Red::EInputKey::IK_NumPad0)))};

    switch (aKey)
    {
    case Red::EInputKey::IK_NumStar: return "*";
    case Red::EInputKey::IK_NumPlus: return "+";
    case Red::EInputKey::IK_NumMinus: return "-";
    case Red::EInputKey::IK_NumPeriod: return ".";
    case Red::EInputKey::IK_NumSlash: return "/";
    case Red::EInputKey::IK_Semicolon: return shift ? ":" : ";";
    case Red::EInputKey::IK_Equals: return shift ? "+" : "=";
    case Red::EInputKey::IK_Comma: return shift ? "<" : ",";
    case Red::EInputKey::IK_Minus: return shift ? "_" : "-";
    case Red::EInputKey::IK_Period: return shift ? ">" : ".";
    case Red::EInputKey::IK_Slash: return shift ? "?" : "/";
    case Red::EInputKey::IK_Tilde: return shift ? "~" : "`";
    case Red::EInputKey::IK_LeftBracket: return shift ? "{" : "[";
    case Red::EInputKey::IK_Backslash: return shift ? "|" : "\\";
    case Red::EInputKey::IK_RightBracket: return shift ? "}" : "]";
    case Red::EInputKey::IK_SingleQuote: return shift ? "\"" : "'";
    default: return {};
    }
#else
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    // if (keyboardState[static_cast<size_t>(Red::EInputKey::IK_LControl)]
    //     || keyboardState[static_cast<size_t>(Red::EInputKey::IK_RControl)])
    //     return {};

    WCHAR buffer[5]{};
    const auto size = ToUnicode(static_cast<UINT>(aKey), 0, keyboardState, buffer, 4, 0x4);

    if (size <= 0)
        return {};

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(buffer);
#endif
}

bool App::WidgetInputService::IsCharacterInput(Red::EInputKey aKey)
{
    return aKey == Red::EInputKey::IK_Space
        || (aKey >= Red::EInputKey::IK_0 && aKey <= Red::EInputKey::IK_9)
        || (aKey >= Red::EInputKey::IK_A && aKey <= Red::EInputKey::IK_Z)
        || (aKey >= Red::EInputKey::IK_NumPad0 && aKey <= Red::EInputKey::IK_NumSlash)
        || (aKey >= Red::EInputKey::IK_Semicolon && aKey <= Red::EInputKey::IK_Tilde)
        || (aKey >= Red::EInputKey::IK_LeftBracket && aKey <= Red::EInputKey::IK_SingleQuote);
}
