#pragma once

#include "App/MacOSUnavailable.hpp"
#include "Red/WorldNode.hpp"

namespace App
{
struct NodeInstanceEx : Red::worldINodeInstance
{
#if defined(__APPLE__)
    // macOS: the worldINodeInstance field offsets are not verified on macOS (clang can place derived members
    // differently), so the accessors return defaults.
    inline static std::once_flag s_macOSReported;
#endif

    [[nodiscard]] Red::Handle<Red::worldNode> GetNode() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        return Raw::WorldNodeInstance::Node::Ref(this);
    }

    [[nodiscard]] Red::Transform GetTransform() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        return Raw::WorldNodeInstance::Transform::Ref(this);
    }

    [[nodiscard]] Red::Vector3 GetScale() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        return Raw::WorldNodeInstance::Scale::Ref(this);
    }

    [[nodiscard]] Red::GlobalNodeID GetGlobalNodeID() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        return {Raw::WorldNodeInstance::NodeID::Ref(this)};
    }

    [[nodiscard]] Red::GlobalNodeID GetProxyNodeID() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        auto setup = Raw::WorldNodeInstance::SetupInfo::Ref(this);
        return {setup ? (*setup)->proxyNodeID : 0};
    }

    [[nodiscard]] float GetStreamingDistance() const
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldINodeInstance node data (GetNode, GetTransform, ...)");
        return {};
#endif
        auto setup = Raw::WorldNodeInstance::SetupInfo::Ref(this);
        return setup ? (*setup)->streamingDistance : 0.0f;
    }
};
}

RTTI_EXPAND_CLASS(Red::worldINodeInstance, App::NodeInstanceEx, {
    RTTI_METHOD(GetNode);
    RTTI_METHOD(GetTransform);
    RTTI_METHOD(GetScale);
    RTTI_METHOD(GetGlobalNodeID);
    RTTI_METHOD(GetProxyNodeID);
    RTTI_METHOD(GetStreamingDistance);
});
