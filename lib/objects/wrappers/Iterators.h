#pragma once

/// \file Iterators.h
/// \brief Iterator adapters
/// \author Pavel Sevecek (sevecek at sirrah.troja.mff.cuni.cz)
/// \date 2016-2017

#include "common/Assert.h"
#include "common/Traits.h"
#include "objects/containers/Tuple.h"
#include <iterator>

NAMESPACE_SPH_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Iterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Simple (forward) iterator over continuous array of objects of type T. Can be used with STL algorithms.
template <typename T, typename TCounter = Size>
class Iterator {
protected:
    using TValue = typename UnwrapReferenceType<T>::Type;

    T* data;
#ifdef SPH_DEBUG
    const T *begin, *end;
#endif

#ifndef SPH_DEBUG
    Iterator(T* data)
        : data(data) {}
#endif
public:
    Iterator() = default;

    Iterator(T* data, const T* UNUSED_IN_RELEASE(begin), const T* UNUSED_IN_RELEASE(end))
        : data(data)
#ifdef SPH_DEBUG
        , begin(begin)
        , end(end)
#endif
    {
    }

    const TValue& operator*() const {
        ASSERT(data >= begin && data < end);
        return *data;
    }
    TValue& operator*() {
        ASSERT(data >= begin && data < end);
        return *data;
    }
    T* operator->() {
        ASSERT(data >= begin && data < end);
        return data;
    }
    const T* operator->() const {
        ASSERT(data >= begin && data < end);
        return data;
    }

#ifdef SPH_DEBUG
    Iterator operator+(const TCounter n) const {
        return Iterator(data + n, begin, end);
    }
    Iterator operator-(const TCounter n) const {
        return Iterator(data - n, begin, end);
    }
#else
    Iterator operator+(const TCounter n) const {
        return Iterator(data + n);
    }
    Iterator operator-(const TCounter n) const {
        return Iterator(data - n);
    }
#endif
    void operator+=(const TCounter n) {
        data += n;
    }
    void operator-=(const TCounter n) {
        data -= n;
    }
    Iterator& operator++() {
        ++data;
        return *this;
    }
    Iterator operator++(int) {
        Iterator tmp(*this);
        operator++();
        return tmp;
    }
    Iterator& operator--() {
        --data;
        return *this;
    }
    Iterator operator--(int) {
        Iterator tmp(*this);
        operator--();
        return tmp;
    }
    Size operator-(const Iterator& iter) const {
        ASSERT(data >= iter.data);
        return data - iter.data;
    }
    bool operator<(const Iterator& iter) const {
        return data < iter.data;
    }
    bool operator>(const Iterator& iter) const {
        return data > iter.data;
    }
    bool operator<=(const Iterator& iter) const {
        return data <= iter.data;
    }
    bool operator>=(const Iterator& iter) const {
        return data >= iter.data;
    }
    bool operator==(const Iterator& iter) const {
        return data == iter.data;
    }
    bool operator!=(const Iterator& iter) const {
        return data != iter.data;
    }


    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = Size;
    using pointer = T*;
    using reference = T&;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ReverseIterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Generic reverse iterator over continuous array of objects of type T.
template <typename T, typename TCounter = Size>
class ReverseIterator {
protected:
    Iterator<T, Size> iter;

public:
    ReverseIterator() = default;

    ReverseIterator(Iterator<T, Size> iter)
        : iter(iter) {}

    decltype(auto) operator*() const {
        return *iter;
    }
    decltype(auto) operator*() {
        return *iter;
    }
    ReverseIterator& operator++() {
        --iter;
        return *this;
    }
    ReverseIterator operator++(int) {
        ReverseIterator tmp(*this);
        operator++();
        return tmp;
    }
    ReverseIterator& operator--() {
        ++iter;
        return *this;
    }
    ReverseIterator operator--(int) {
        ReverseIterator tmp(*this);
        operator--();
        return tmp;
    }
    bool operator==(const ReverseIterator& other) const {
        return iter == other.iter;
    }
    bool operator!=(const ReverseIterator& other) const {
        return iter != other.iter;
    }
};

/// Creates reverse iterator by wrapping forward iterator, utilizes type deduction.
template <typename T, typename TCounter>
ReverseIterator<T, TCounter> reverseIterator(const Iterator<T, TCounter> iter) {
    return ReverseIterator<T, TCounter>(iter);
}


/// Wrapper of generic container allowing to iterate over its elements in reverse order. The wrapper can hold
/// l-value reference, or the container can be moved into the wrapper.
template <typename TContainer>
class ReverseAdapter {
private:
    TContainer container;

public:
    template <typename T>
    ReverseAdapter(T&& container)
        : container(std::forward<T>(container)) {}

    /// Returns iterator pointing to the last element in container.
    auto begin() {
        return reverseIterator(container.end() - 1);
    }

    /// Returns iterator pointiing to one before the first element.
    auto end() {
        return reverseIterator(container.begin() - 1);
    }

    auto size() const {
        return container.size();
    }
};

/// Creates the ReverseAdapter over given container, utilizing type deduction.
/// To iterate over elements of container in reverse order, use
/// \code
/// for (T& value : reverse(container)) {
///    // do something with value
/// }
/// \endcode
template <typename TContainer>
ReverseAdapter<TContainer> reverse(TContainer&& container) {
    return ReverseAdapter<TContainer>(std::forward<TContainer>(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ComponentIterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Provides iterator over selected component of vector array or any iterable container holding vectors. Can
/// be used in STL algorithms, namely in std::sort.
template <typename TIterator>
class ComponentIterator {
private:
    TIterator iterator;
    Size component;

    using T = decltype((*std::declval<TIterator>())[std::declval<Size>()]);
    using RawT = std::decay_t<T>;

public:
    ComponentIterator() = default;

    ComponentIterator(const TIterator& iterator, const int component)
        : iterator(iterator)
        , component(component) {}

    const RawT& operator*() const {
        return (*iterator)[component];
    }

    RawT& operator*() {
        return (*iterator)[component];
    }

    ComponentIterator& operator++() {
        ++iterator;
        return *this;
    }
    ComponentIterator operator++(int) {
        ComponentIterator tmp(*this);
        operator++();
        return tmp;
    }
    ComponentIterator& operator--() {
        --iterator;
        return *this;
    }
    ComponentIterator operator--(int) {
        ComponentIterator tmp(*this);
        operator--();
        return tmp;
    }


    ComponentIterator operator+(const int n) const {
        return ComponentIterator(iterator + n, component);
    }
    ComponentIterator operator-(const int n) const {
        return ComponentIterator(iterator - n, component);
    }
    void operator+=(const int n) {
        iterator += n;
    }
    void operator-=(const int n) {
        iterator -= n;
    }

    Size operator-(const ComponentIterator& other) const {
        return iterator - other.iterator;
    }
    bool operator<(const ComponentIterator& other) const {
        return iterator < other.iterator;
    }
    bool operator>(const ComponentIterator& other) const {
        return iterator > other.iterator;
    }
    bool operator<=(const ComponentIterator& other) const {
        return iterator <= other.iterator;
    }
    bool operator>=(const ComponentIterator& other) const {
        return iterator >= other.iterator;
    }
    bool operator==(const ComponentIterator& other) {
        return iterator == other.iterator;
    }
    bool operator!=(const ComponentIterator& other) {
        return iterator != other.iterator;
    }

    using iterator_category = std::random_access_iterator_tag;
    using value_type = RawT;
    using difference_type = Size;
    using pointer = RawT*;
    using reference = RawT&;
};


/// Wraps a vector container, providing means to iterate over given component of vector elements.
template <typename TBuffer>
struct VectorComponentAdapter {
    TBuffer data;
    const Size component;

    using TIterator = decltype(std::declval<TBuffer>().begin());

    VectorComponentAdapter(TBuffer&& data, const Size component)
        : data(std::forward<TBuffer>(data))
        , component(component) {}

    auto begin() {
        return ComponentIterator<TIterator>(data.begin(), component);
    }

    auto end() {
        return ComponentIterator<TIterator>(data.end(), component);
    }
};

/// Returns the VectorComponentAdapter, utilizing type deduction. Indended usage is:
/// \code
/// Array<Vector> array(20);
/// for (Float& value : componentAdapter(value, 0)) {
///    value = 5; // set x-components of vectors to 5
/// }
/// \endcode
template <typename TBuffer>
auto componentAdapter(TBuffer&& buffer, const Size component) {
    return VectorComponentAdapter<TBuffer>(std::forward<TBuffer>(buffer), component);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TupleIterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Holds multiple iterators, advancing all of them at the same time. Has only the necessary functions to use
/// in range-based for loops.
template <typename TElement, typename... TIterator>
class TupleIterator {
private:
    Tuple<TIterator...> iterators;

public:
    TupleIterator(const TIterator&... iters)
        : iterators(iters...) {}

    TupleIterator& operator++() {
        forEach(iterators, [](auto& iter) { ++iter; });
        return *this;
    }

    TElement operator*() {
        return apply(iterators, [](auto&... values) -> TElement { return { *values... }; });
    }

    const TElement operator*() const {
        return apply(iterators, [](const auto&... values) -> TElement { return { *values... }; });
    }

    bool operator==(const TupleIterator& other) const {
        // all iterators have the same range, so we can simply compare first one
        return iterators.template get<0>() != other.iterators.template get<0>();
    }

    bool operator!=(const TupleIterator& other) const {
        // all iterators have the same range, so we can simply compare first one
        return iterators.template get<0>() != other.iterators.template get<0>();
    }
};

/// Creates TupleIterator from individual iterators, utilizing type deduction.
template <typename TElement, typename... TIterators>
TupleIterator<TElement, TIterators...> makeTupleIterator(const TIterators&... iterators) {
    return TupleIterator<TElement, TIterators...>(iterators...);
}

/// Wraps any number of containers, providing means to iterate over all of them at the same time. This can
/// only be used for containers of the same size.
template <typename TElement, typename... TContainers>
class TupleAdapter {
private:
    Tuple<TContainers...> tuple;

public:
    TupleAdapter(TContainers&&... containers)
        : tuple(std::forward<TContainers>(containers)...) {
        ASSERT(tuple.size() > 1);
#ifdef SPH_DEBUG
        // check that all containers are of the same size
        const Size size0 = tuple.template get<0>().size();
        forEach(tuple, [size0](auto& container) { ASSERT(container.size() == size0); });
#endif
    }

    auto begin() {
        return apply(
            tuple, [](auto&... containers) { return makeTupleIterator<TElement>(containers.begin()...); });
    }

    auto begin() const {
        return apply(tuple,
            [](const auto&... containers) { return makeTupleIterator<TElement>(containers.begin()...); });
    }

    auto end() {
        return apply(
            tuple, [](auto&... containers) { return makeTupleIterator<TElement>(containers.end()...); });
    }

    auto end() const {
        return apply(tuple,
            [](const auto&... containers) { return makeTupleIterator<TElement>(containers.end()...); });
    }

    Size size() const {
        return tuple.template get<0>().size();
    }
};

template <typename TElement, typename... TContainers>
TupleAdapter<TElement, TContainers...> iterateTuple(TContainers&&... containers) {
    return TupleAdapter<TElement, TContainers...>(std::forward<TContainers>(containers)...);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IteratorWithIndex
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename TValue>
class ElementWithIndex {
private:
    TValue data;
    Size idx;

public:
    ElementWithIndex(TValue&& value, const Size index)
        : data(std::forward<TValue>(value))
        , idx(index) {}

    INLINE TValue& value() {
        return data;
    }

    INLINE const TValue& value() const {
        return data;
    }

    INLINE operator TValue&() {
        return value();
    }

    INLINE operator const TValue&() const {
        return value();
    }

    INLINE Size index() const {
        return idx;
    }
};

template <typename TValue>
ElementWithIndex<TValue> makeElementWithIndex(TValue&& value, const Size index) {
    return ElementWithIndex<TValue>(std::forward<TValue>(value), index);
}

/// Wrapper of iterator keeping also an index of current element.
template <typename TIterator>
class IteratorWithIndex {
private:
    TIterator iterator;
    Size index;

    using TValue = decltype(*std::declval<TIterator>());

public:
    IteratorWithIndex(const TIterator iterator, const Size index)
        : iterator(iterator)
        , index(index) {}

    ElementWithIndex<TValue> operator*() {
        return makeElementWithIndex(*iterator, index);
    }

    ElementWithIndex<const TValue> operator*() const {
        return makeElementWithIndex(*iterator, index);
    }

    IteratorWithIndex& operator++() {
        ++iterator;
        ++index;
        return *this;
    }

    bool operator!=(const IteratorWithIndex& other) const {
        return iterator != other.iterator;
    }
};

template <typename TIterator>
IteratorWithIndex<TIterator> makeIteratorWithIndex(TIterator&& iterator, const Size index) {
    return IteratorWithIndex<TIterator>(std::forward<TIterator>(iterator), index);
}


template <typename TContainer>
class IndexAdapter {
private:
    TContainer container;

public:
    IndexAdapter(TContainer&& container)
        : container(std::forward<TContainer>(container)) {}

    auto begin() {
        return makeIteratorWithIndex(container.begin(), 0);
    }

    auto end() {
        return makeIteratorWithIndex(container.end(), container.size());
    }
};

template <typename TContainer>
IndexAdapter<TContainer> iterateWithIndex(TContainer&& container) {
    return IndexAdapter<TContainer>(std::forward<TContainer>(container));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SubsetIterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Allows to iterate over a given subset of a container, given by condition functor.
template <typename TIterator, typename TCondition>
class SubsetIterator {
private:
    TIterator iter;
    TIterator end;
    TCondition condition;

public:
    SubsetIterator(const TIterator& iterator, const TIterator& end, TCondition&& condition)
        : iter(iterator)
        , end(end)
        , condition(std::forward<TCondition>(condition)) {
        // move to first element of the subset
        while (iter != end && !condition(*iter)) {
            ++iter;
        }
    }

    INLINE SubsetIterator& operator++() {
        do {
            ++iter;
        } while (iter != end && !condition(*iter));
        return *this;
    }

    INLINE decltype(auto) operator*() {
        ASSERT(iter != end);
        return *iter;
    }

    INLINE decltype(auto) operator*() const {
        ASSERT(iter != end);
        return *iter;
    }

    INLINE bool operator==(const SubsetIterator& other) {
        return iter == other.iter;
    }

    INLINE bool operator!=(const SubsetIterator& other) {
        return iter != other.iter;
    }
};

/// \todo test
template <typename TIterator, typename TCondition>
INLINE auto makeSubsetIterator(const TIterator& iterator, const TIterator& end, TCondition&& condition) {
    return SubsetIterator<TIterator, TCondition>(iterator, end, std::forward<TCondition>(condition));
}

/// Non-owning view to access and iterate over subset of container
template <typename TContainer, typename TCondition>
class SubsetAdapter {
private:
    TContainer container;
    TCondition condition;

public:
    SubsetAdapter(TContainer&& container, TCondition&& condition)
        : container(std::forward<TContainer>(container))
        , condition(std::forward<TCondition>(condition)) {}

    auto begin() {
        return makeSubsetIterator(container.begin(), container.end(), condition);
    }

    auto end() {
        return makeSubsetIterator(container.end(), container.end(), condition);
    }
};

/// \todo test
template <typename TContainer, typename TCondition>
auto subset(TContainer&& container, TCondition&& condition) {
    return SubsetAdapter<TContainer, TCondition>(
        std::forward<TContainer>(container), std::forward<TCondition>(condition));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// IndexIterator
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


class IndexIterator {
protected:
    Size idx;

public:
    INLINE IndexIterator(const Size idx)
        : idx(idx) {}

    INLINE Size operator*() const {
        return idx;
    }

    INLINE IndexIterator& operator++() {
        ++idx;
        return *this;
    }

    INLINE bool operator!=(const IndexIterator other) const {
        return idx != other.idx;
    }
};

class IndexSequence {
protected:
    Size from;
    Size to;

public:
    INLINE IndexSequence(const Size from, const Size to)
        : from(from)
        , to(to) {
        ASSERT(from <= to);
    }

    INLINE IndexIterator begin() const {
        return IndexIterator(from);
    }

    INLINE IndexIterator end() const {
        return IndexIterator(to);
    }

    INLINE Size size() const {
        return to - from;
    }

    INLINE bool operator==(const IndexSequence& other) {
        return from == other.from && to == other.to;
    }
};

NAMESPACE_SPH_END
