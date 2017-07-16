#pragma once

/// \file RawPtr.h
/// \brief Simple non-owning wrapper of pointer
/// \author Pavel Sevecek (sevecek at sirrah.troja.mff.cuni.cz)
/// \date 2016-2017

#include "common/Assert.h"

NAMESPACE_SPH_BEGIN

/// \brief Non-owning wrapper of pointer
///
/// Inspired by proposal of std::observer_ptr. It mainly serves for self-documentation, clearly expressing
/// ownership (or non-ownership in this case) of the pointer. Unlike raw pointer, it is initialized to nullptr
/// for convenience. This is a slight overhead compared to raw pointer, negligible in most use cases. When
/// deferencing the pointer, it checks for nullptr using assert.
template <typename T>
class RawPtr {
private:
    T* ptr;

public:
    INLINE RawPtr()
        : ptr(nullptr) {}

    INLINE RawPtr(std::nullptr_t)
        : ptr(nullptr) {}

    template <typename T2, typename = std::enable_if_t<std::is_convertible<T2*, T*>::value>>
    INLINE RawPtr(T2* ptr)
        : ptr(ptr) {}

    INLINE RawPtr& operator=(std::nullptr_t) {
        ptr = nullptr;
        return *this;
    }

    template <typename T2, typename = std::enable_if_t<std::is_convertible<T2*, T*>::value>>
    INLINE RawPtr& operator=(T2* rhs) {
        ptr = rhs;
        return *this;
    }

    INLINE T& operator*() const {
        ASSERT(ptr);
        return *ptr;
    }

    INLINE T* operator->() const {
        ASSERT(ptr);
        return ptr;
    }

    INLINE explicit operator bool() const {
        return ptr != nullptr;
    }

    INLINE bool operator!() const {
        return !ptr;
    }

    INLINE T* get() const {
        return ptr;
    }
};

template <typename T>
INLINE bool operator==(const RawPtr<T> lhs, std::nullptr_t) {
    return !lhs;
}

template <typename T>
INLINE bool operator==(std::nullptr_t, const RawPtr<T> rhs) {
    return !rhs;
}

template <typename T>
INLINE bool operator!=(const RawPtr<T> lhs, std::nullptr_t) {
    return bool(lhs);
}

template <typename T>
INLINE bool operator!=(std::nullptr_t, const RawPtr<T> rhs) {
    return bool(rhs);
}

template <typename T1, typename T2>
INLINE bool operator==(const RawPtr<T1> lhs, const RawPtr<T2> rhs) {
    return lhs.get() == rhs.get();
}

template <typename T1, typename T2>
INLINE bool operator!=(const RawPtr<T1> lhs, const RawPtr<T2> rhs) {
    return lhs.get() != rhs.get();
}

NAMESPACE_SPH_END
