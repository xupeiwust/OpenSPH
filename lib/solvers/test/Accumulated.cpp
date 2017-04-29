#include "solvers/Accumulated.h"
#include "catch.hpp"
#include "objects/containers/PerElementWrapper.h"
#include "utils/SequenceTest.h"
#include "utils/Utils.h"

using namespace Sph;

TEST_CASE("Accumulated sum simple", "[accumulated]") {
    Accumulated ac1;
    REQUIRE_ASSERT(ac1.getValue<Size>(QuantityId::NEIGHBOUR_CNT));
    REQUIRE(ac1.getBufferCnt() == 0);
    ac1.insert<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(ac1.getBufferCnt() == 1);
    // subsequent calls dont do anything
    ac1.insert<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(ac1.getBufferCnt() == 1);

    ac1.initialize(5);
    ArrayView<Size> buffer1 = ac1.getValue<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(buffer1.size() == 5);
    REQUIRE(ac1.getValue<Size>(QuantityId::NEIGHBOUR_CNT).size() == 5);
    REQUIRE(ac1.getBufferCnt() == 1);

    Accumulated ac2;
    ac2.insert<Size>(QuantityId::NEIGHBOUR_CNT);
    ac2.initialize(5);
    ArrayView<Size> buffer2 = ac2.getValue<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(ac2.getBufferCnt() == 1);
    for (Size i = 0; i < 5; ++i) {
        buffer1[i] = i;
        buffer2[i] = 5 - i;
    }

    ac1.sum(ac2);
    REQUIRE(perElement(buffer1) == 5);
}

template <typename TValue>
ArrayView<TValue> getInserted(Accumulated& ac, const QuantityId id, const Size size) {
    ac.insert<TValue>(id);
    ac.initialize(size); // multiple initialization do not matter, it's only a bit inefficient
    return ac.getValue<TValue>(id);
}

static Accumulated getAccumulated() {
    Accumulated ac;
    ArrayView<Size> buffer1 = getInserted<Size>(ac, QuantityId::NEIGHBOUR_CNT, 5);
    ArrayView<Float> buffer2 = getInserted<Float>(ac, QuantityId::DENSITY, 5);
    ArrayView<Vector> buffer3 = getInserted<Vector>(ac, QuantityId::ENERGY, 5);
    ArrayView<Tensor> buffer4 = getInserted<Tensor>(ac, QuantityId::POSITIONS, 5);
    for (Size i = 0; i < 5; ++i) {
        buffer1[i] = 5;
        buffer2[i] = 3._f;
        buffer3[i] = Vector(2._f);
        buffer4[i] = Tensor(1._f);
    }
    return ac;
}

static Storage getStorage() {
    Storage storage;
    storage.insert<Size>(QuantityId::NEIGHBOUR_CNT, OrderEnum::ZERO, Array<Size>{ 1 });
    storage.insert<Float>(QuantityId::DENSITY, OrderEnum::ZERO, 0._f);
    storage.insert<Vector>(QuantityId::ENERGY, OrderEnum::ZERO, Vector(0._f));
    storage.insert<Tensor>(QuantityId::POSITIONS, OrderEnum::ZERO, Tensor::null());
    return storage;
}

TEST_CASE("Accumulated sum parallelized", "[accumulated]") {
    Accumulated ac1 = getAccumulated();
    Accumulated ac2 = getAccumulated();
    ThreadPool pool;
    ac1.sum(pool, ac2);
    Storage storage = getStorage();
    ac1.store(storage);

    REQUIRE(storage.getQuantityCnt() == 4);
    REQUIRE(storage.getParticleCnt() == 5);
    ArrayView<Size> buffer1 = storage.getValue<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(buffer1.size() == 5);
    REQUIRE(perElement(buffer1) == 10);
    ArrayView<Float> buffer2 = storage.getValue<Float>(QuantityId::DENSITY);
    REQUIRE(buffer2.size() == 5);
    REQUIRE(perElement(buffer2) == 6._f);
    ArrayView<Vector> buffer3 = storage.getValue<Vector>(QuantityId::ENERGY);
    REQUIRE(buffer3.size() == 5);
    REQUIRE(perElement(buffer3) == Vector(4._f));
    ArrayView<Tensor> buffer4 = storage.getValue<Tensor>(QuantityId::POSITIONS);
    REQUIRE(buffer4.size() == 5);
    REQUIRE(perElement(buffer4) == Tensor(2._f));
}

TEST_CASE("Accumulated store", "[accumulated]") {
    Accumulated ac;
    ArrayView<Size> buffer1 = getInserted<Size>(ac, QuantityId::NEIGHBOUR_CNT, 5);
    for (Size i = 0; i < 5; ++i) {
        buffer1[i] = i;
    }
    Storage storage = getStorage();
    ac.store(storage);
    ArrayView<Size> buffer2 = storage.getValue<Size>(QuantityId::NEIGHBOUR_CNT);
    REQUIRE(buffer2.size() == 5);
    for (Size i = 0; i < 5; ++i) {
        REQUIRE(buffer2[i] == i);
    }
}
