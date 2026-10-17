#pragma once

#include "App/MacOSUnavailable.hpp"
#include "App/World/NodeSetupWrapper.hpp"
#include "Red/StreamingSector.hpp"

namespace App
{
struct StreamingSectorEx : Red::worldStreamingSector
{
#if defined(__APPLE__)
    // macOS: worldStreamingSector derives from CResource, whose tail padding clang reuses, and the node buffer offset
    // (+0x40) is not verified on macOS, so these return defaults.
    inline static std::once_flag s_macOSReported;
#endif

    Red::DynArray<Red::Handle<Red::worldNode>> GetNodes()
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        return buffer.nodes;
    }

    int32_t GetNodeCount()
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        return static_cast<int32_t>(buffer.nodes.Size());
    }

    Red::Handle<Red::worldNode> GetNode(int32_t aIndex)
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        if (aIndex >= 0 && aIndex < buffer.nodes.Size())
            return buffer.nodes[aIndex];
        return {};
    }

    int32_t GetNodeSetupCount()
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        return static_cast<int32_t>(buffer.nodeSetups.GetInstanceCount());
    }

    Red::Handle<WorldNodeSetupWrapper> GetNodeSetup(int32_t aIndex)
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto* buffer = Raw::StreamingSector::NodeBuffer::Ptr(this);
        if (aIndex >= 0 && aIndex < buffer->nodeSetups.GetInstanceCount())
            return Red::MakeHandle<WorldNodeSetupWrapper>(buffer, buffer->nodeSetups.GetInstance(aIndex));
        return {};
    }
    Red::DynArray<Red::NodeRef> GetNodeRefs()
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        return buffer.nodeRefs;
    }

    int32_t GetNodeRefCount()
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        return static_cast<int32_t>(buffer.nodeRefs.Size());
    }

    Red::NodeRef GetNodeRef(int32_t aIndex)
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return {};
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        if (aIndex >= 0 && aIndex < buffer.nodeRefs.Size())
            return buffer.nodeRefs[aIndex];
        return {};
    }

    void SetNodeRef(int32_t aIndex, Red::NodeRef aNodeRef)
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return;
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        if (aIndex >= 0 && aIndex < buffer.nodeRefs.Size())
        {
            buffer.nodeRefs[aIndex] = aNodeRef;
        }
    }

    void AddNodeRef(Red::NodeRef aNodeRef)
    {
#if defined(__APPLE__)
        ReportUnavailableOnMacOS(s_macOSReported, "worldStreamingSector node buffer (GetNodes, GetNodeRefs, ...)");
        return;
#endif
        auto& buffer = Raw::StreamingSector::NodeBuffer::Ref(this);
        buffer.nodeRefs.PushBack(aNodeRef);
    }
};
}

RTTI_EXPAND_CLASS(Red::worldStreamingSector, App::StreamingSectorEx, {
    RTTI_METHOD(GetNodes);
    RTTI_METHOD(GetNodeCount);
    RTTI_METHOD(GetNode);
    RTTI_METHOD(GetNodeSetupCount);
    RTTI_METHOD(GetNodeSetup);
    RTTI_METHOD(GetNodeRefs);
    RTTI_METHOD(GetNodeRefCount);
    RTTI_METHOD(GetNodeRef);
});
