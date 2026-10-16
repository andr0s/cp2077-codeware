#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#if defined(_WIN32) || defined(_WIN64)
#include <TiltedCore/StlAllocator.hpp>
#include <tsl/hopscotch_map.h>
#include <tsl/hopscotch_set.h>
#endif

namespace Core
{
// Pretty much the same as TiltedCore with StlAllocator,
// but unique ptr allows implicit casting to the base.
// On macOS, we use standard allocators instead.

namespace Detail
{
template<class T>
struct UniqueDeleter
{
    constexpr UniqueDeleter() noexcept = default;

    template<class U>
    requires std::is_base_of_v<T, U>
    UniqueDeleter(const UniqueDeleter<U>& d) noexcept {}

    void operator()(std::conditional_t<std::is_array_v<T>, T, T*> aData)
    {
#if defined(_WIN32) || defined(_WIN64)
        TiltedPhoques::Delete<T>(aData);
#else
        delete aData;
#endif
    }
};
}

#if defined(_WIN32) || defined(_WIN64)
// Windows: Use TiltedPhoques allocators and hopscotch containers
template<class T>
using Vector = std::vector<T, TiltedPhoques::StlAllocator<T>>;

template<class T>
using Set = tsl::hopscotch_set<T, std::hash<T>, std::equal_to<T>, TiltedPhoques::StlAllocator<T>>;

template<class T, class U>
using Map = tsl::hopscotch_map<T, U, std::hash<T>, std::equal_to<T>, TiltedPhoques::StlAllocator<std::pair<T, U>>>;

template<class T, class U>
using SortedMap = std::map<T, U, std::less<T>, TiltedPhoques::StlAllocator<std::pair<const T, U>>>;

#else
// macOS: Use standard containers
template<class T>
using Vector = std::vector<T>;

template<class T>
using Set = std::unordered_set<T>;

namespace Detail
{
// The shared code uses the tsl::hopscotch_map iterator accessors key() and value(). This adds them to the
// std::unordered_map iterators, so that code compiles unchanged. value() is mutable for a mutable iterator, as in tsl.
template<class TIterator>
class MapIterator : public TIterator
{
public:
    MapIterator() = default;

    MapIterator(const TIterator& aIterator)
        : TIterator(aIterator)
    {
    }

    template<class TOther>
    requires (!std::is_same_v<TOther, TIterator>) && std::is_convertible_v<TOther, TIterator>
    MapIterator(const MapIterator<TOther>& aOther)
        : TIterator(static_cast<const TOther&>(aOther))
    {
    }

    decltype(auto) key() const
    {
        return (TIterator::operator->()->first);
    }

    decltype(auto) value() const
    {
        return (TIterator::operator->()->second);
    }
};
}

template<class T, class U>
class Map : public std::unordered_map<T, U>
{
    using Base = std::unordered_map<T, U>;

public:
    using iterator = Detail::MapIterator<typename Base::iterator>;
    using const_iterator = Detail::MapIterator<typename Base::const_iterator>;

    using Base::Base;
    using Base::operator=;

    iterator begin() noexcept
    {
        return Base::begin();
    }

    const_iterator begin() const noexcept
    {
        return Base::begin();
    }

    const_iterator cbegin() const noexcept
    {
        return Base::cbegin();
    }

    iterator end() noexcept
    {
        return Base::end();
    }

    const_iterator end() const noexcept
    {
        return Base::end();
    }

    const_iterator cend() const noexcept
    {
        return Base::cend();
    }

    iterator find(const T& aKey)
    {
        return Base::find(aKey);
    }

    const_iterator find(const T& aKey) const
    {
        return Base::find(aKey);
    }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args&&... aArgs)
    {
        auto result = Base::emplace(std::forward<Args>(aArgs)...);
        return {result.first, result.second};
    }

    template<class... Args>
    std::pair<iterator, bool> try_emplace(const T& aKey, Args&&... aArgs)
    {
        auto result = Base::try_emplace(aKey, std::forward<Args>(aArgs)...);
        return {result.first, result.second};
    }

    template<class... Args>
    std::pair<iterator, bool> try_emplace(T&& aKey, Args&&... aArgs)
    {
        auto result = Base::try_emplace(std::move(aKey), std::forward<Args>(aArgs)...);
        return {result.first, result.second};
    }

    template<class V>
    std::pair<iterator, bool> insert_or_assign(const T& aKey, V&& aValue)
    {
        auto result = Base::insert_or_assign(aKey, std::forward<V>(aValue));
        return {result.first, result.second};
    }

    template<class V>
    std::pair<iterator, bool> insert_or_assign(T&& aKey, V&& aValue)
    {
        auto result = Base::insert_or_assign(std::move(aKey), std::forward<V>(aValue));
        return {result.first, result.second};
    }

    std::pair<iterator, bool> insert(const typename Base::value_type& aValue)
    {
        auto result = Base::insert(aValue);
        return {result.first, result.second};
    }

    std::pair<iterator, bool> insert(typename Base::value_type&& aValue)
    {
        auto result = Base::insert(std::move(aValue));
        return {result.first, result.second};
    }

    template<class P>
    requires std::is_constructible_v<typename Base::value_type, P&&>
    std::pair<iterator, bool> insert(P&& aValue)
    {
        auto result = Base::insert(std::forward<P>(aValue));
        return {result.first, result.second};
    }

    using Base::erase;

    iterator erase(iterator aPos)
    {
        return Base::erase(static_cast<const typename Base::iterator&>(aPos));
    }

    iterator erase(const_iterator aPos)
    {
        return Base::erase(static_cast<const typename Base::const_iterator&>(aPos));
    }
};

template<class T, class U>
using SortedMap = std::map<T, U>;
#endif

template<class T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
using SortedSet = std::set<T, Compare, Allocator>;

template<class T>
using SharedPtr = std::shared_ptr<T>;

template<class T>
using WeakPtr = std::weak_ptr<T>;

template<class T>
using UniquePtr = std::unique_ptr<T, Detail::UniqueDeleter<T>>;

template<typename T>
struct ShareFromThis : public std::enable_shared_from_this<T>
{
    SharedPtr<T> ToShared()
    {
        return std::enable_shared_from_this<T>::shared_from_this();
    }

    WeakPtr<T> ToWeak()
    {
        return std::enable_shared_from_this<T>::weak_from_this();
    }
};

template<typename T, typename... Args>
auto MakeShared(Args&&... aArgs)
{
#if defined(_WIN32) || defined(_WIN64)
    return std::allocate_shared<T>(TiltedPhoques::StlAllocator<T>(), std::forward<Args>(aArgs)...);
#else
    return std::make_shared<T>(std::forward<Args>(aArgs)...);
#endif
}

template<typename T, typename... Args>
auto MakeUnique(Args&&... aArgs)
{
#if defined(_WIN32) || defined(_WIN64)
    return UniquePtr<T>(TiltedPhoques::New<T>(std::forward<Args>(aArgs)...));
#else
    return UniquePtr<T>(new T(std::forward<Args>(aArgs)...));
#endif
}
}
