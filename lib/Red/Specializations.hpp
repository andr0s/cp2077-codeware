#pragma once

#include "TypeInfo/Resolving.hpp"

template<>
struct std::hash<RED4ext::CName>
{
    std::size_t operator()(RED4ext::CName aKey) const
    {
        return aKey.hash;
    }
};

template<>
struct std::hash<RED4ext::TweakDBID>
{
    std::size_t operator()(RED4ext::TweakDBID aKey) const
    {
        return aKey.value;
    }
};

template<>
struct std::hash<RED4ext::ResourcePath>
{
    std::size_t operator()(RED4ext::ResourcePath aKey) const
    {
        return aKey.hash;
    }
};

template<>
struct std::hash<RED4ext::NodeRef>
{
    std::size_t operator()(RED4ext::NodeRef aKey) const
    {
        return aKey.hash;
    }
};

#if defined(__APPLE__)
// macOS: size_t (unsigned long) and uint64_t (unsigned long long) are different types, so a key type with both
// uint32_t and uint64_t conversions (EntityID) is not implicitly convertible to size_t. Hash through uint64_t, the
// conversion the Windows build ends up calling.
template<typename T>
requires std::is_class_v<T> && std::is_convertible_v<T, uint64_t> && Red::Detail::HasGeneratedTypeName<T>
struct std::hash<T>
{
    std::size_t operator()(T aKey) const
    {
        return static_cast<size_t>(static_cast<uint64_t>(aKey));
    }
};
#else
template<typename T>
requires std::is_class_v<T> && std::is_convertible_v<T, size_t> && Red::Detail::HasGeneratedTypeName<T>
struct std::hash<T>
{
    std::size_t operator()(T aKey) const
    {
        return static_cast<size_t>(aKey);
    }
};
#endif
