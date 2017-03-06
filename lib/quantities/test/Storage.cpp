#include "quantities/Storage.h"
#include "catch.hpp"
#include "physics/Eos.h"
#include "quantities/Iterate.h"
#include "quantities/AbstractMaterial.h"
#include "system/Factory.h"
#include "system/Settings.h"

using namespace Sph;

TEST_CASE("Storage resize", "[storage]") {
    Storage storage;
    REQUIRE(storage.getQuantityCnt() == 0);
    REQUIRE(storage.getParticleCnt() == 0);

    storage.insert<Size, OrderEnum::ZERO_ORDER>(QuantityIds::MATERIAL_IDX, Array<Size>{ 0 });
    storage.resize(5);
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY, 3._f);
    REQUIRE(storage.getQuantityCnt() == 2);
    REQUIRE(storage.getParticleCnt() == 5);

    storage.insert<Vector, OrderEnum::SECOND_ORDER>(QuantityIds::MASSES, Vector(5._f));
    REQUIRE(storage.getQuantityCnt() == 3);
    REQUIRE(storage.has(QuantityIds::DENSITY));
    REQUIRE(storage.has(QuantityIds::MASSES));
    REQUIRE(!storage.has(QuantityIds::POSITIONS));
    REQUIRE((storage.has<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY)));
    REQUIRE(!(storage.has<Float, OrderEnum::SECOND_ORDER>(QuantityIds::DENSITY)));
    REQUIRE(!(storage.has<Vector, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY)));

    REQUIRE(storage.getValue<Vector>(QuantityIds::MASSES).size() == 5);
    REQUIRE(storage.getValue<Float>(QuantityIds::DENSITY) == Array<Float>({ 3._f, 3._f, 3._f, 3._f, 3._f }));
}

/*TEST_CASE("Storage emplaceWithFunctor", "[storage]") {
    Storage storage;
    Array<Vector> r{ Vector(0._f), Vector(1._f), Vector(2._f), Vector(4._f) };
    Array<Vector> origR = r.clone();
    storage.insert<Vector, OrderEnum::SECOND_ORDER>(QuantityIds::POSITIONS, std::move(r));

    int idx = 0;
    storage.emplaceWithFunctor<Vector, OrderEnum::FIRST_ORDER>(
        QuantityIds::MASSES, [&idx, &origR](const Vector& v, const int i) {
            REQUIRE(v == origR[idx]);
            REQUIRE(i == idx);
            idx++;

            return Vector(Float(i), 0._f, 0._f);
        });
    REQUIRE(storage.getValue<Vector>(QuantityIds::MASSES) == Array<Vector>({ Vector(0._f, 0._f, 0._f),
                                                                 Vector(1._f, 0._f, 0._f),
                                                                 Vector(2._f, 0._f, 0._f),
                                                                 Vector(3._f, 0._f, 0._f) }));
}*/

TEST_CASE("Clone storages", "[storage]") {
    Storage storage;
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::MATERIAL_IDX, Array<Float>{ 0 });
    storage.resize(5);
    storage.insert<Float, OrderEnum::SECOND_ORDER>(QuantityIds::POSITIONS, 4._f);
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::MASSES, 1._f);
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY, 3._f);


    auto rs = storage.getAll<Float>(QuantityIds::POSITIONS);
    rs[0].resize(6);
    rs[1].resize(5);
    rs[2].resize(4);
    storage.getValue<Float>(QuantityIds::MASSES).resize(3);
    auto rhos = storage.getAll<Float>(QuantityIds::DENSITY);
    rhos[0].resize(2);
    rhos[1].resize(1);

    ArrayView<Float> r, v, dv, m, rho, drho;

    auto updateViews = [&](Storage& st) {
        tie(r, v, dv) = st.getAll<Float>(QuantityIds::POSITIONS);
        tie(rho, drho) = st.getAll<Float>(QuantityIds::DENSITY);
        m = st.getValue<Float>(QuantityIds::MASSES);
    };

    // clone all buffers
    Storage cloned1 = storage.clone(VisitorEnum::ALL_BUFFERS);
    updateViews(cloned1);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(6u, 5u, 4u, 3u, 2u, 1u));

    // only highest derivatives
    Storage cloned2 = storage.clone(VisitorEnum::HIGHEST_DERIVATIVES);
    updateViews(cloned2);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(0u, 0u, 4u, 0u, 0u, 1u));

    // only second derivatives
    Storage cloned3 = storage.clone(VisitorEnum::SECOND_ORDER);
    updateViews(cloned3);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(0u, 0u, 4u, 0u, 0u, 0u));

    // swap all buffers with 1st storage
    cloned3.swap(cloned1, VisitorEnum::ALL_BUFFERS);
    updateViews(cloned3);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(6u, 5u, 4u, 3u, 2u, 1u));
    updateViews(cloned1);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(0u, 0u, 4u, 0u, 0u, 0u));

    cloned3.getAll<Float>(QuantityIds::POSITIONS)[2].resize(12);
    cloned3.swap(cloned1, VisitorEnum::HIGHEST_DERIVATIVES);
    updateViews(cloned3);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(6u, 5u, 4u, 3u, 2u, 0u));
    updateViews(cloned1);
    REQUIRE(makeArray(r.size(), v.size(), dv.size(), m.size(), rho.size(), drho.size()) ==
            makeArray(0u, 0u, 12u, 0u, 0u, 1u));
}

TEST_CASE("Storage merge", "[storage]") {
    Storage storage1;
    storage1.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY, Array<Float>{ 0._f, 1._f });

    Storage storage2;
    storage2.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY, Array<Float>{ 2._f, 3._f });
    storage1.merge(std::move(storage2));

    REQUIRE(storage1.getQuantityCnt() == 1);
    REQUIRE(storage1.getParticleCnt() == 4);

    ArrayView<Float> rho = storage1.getValue<Float>(QuantityIds::DENSITY);
    REQUIRE(rho == makeArray(0._f, 1._f, 2._f, 3._f));
}

TEST_CASE("Storage init", "[storage]") {
    Storage storage;
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::MATERIAL_IDX, Array<Float>{ 0 }); // dummy unit
    storage.resize(3);
    storage.insert<Float, OrderEnum::SECOND_ORDER>(QuantityIds::POSITIONS, 3._f);
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::MASSES, 1._f);
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::DENSITY, 2._f);

    iterate<VisitorEnum::ALL_BUFFERS>(storage, [](auto&& buffer) {
        using Type = typename std::decay_t<decltype(buffer)>::Type;
        buffer.fill(Type(5._f));
    });
    REQUIRE(storage.getAll<Float>(QuantityIds::POSITIONS)[2] == makeArray(5._f, 5._f, 5._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::MASSES)[1] == makeArray(5._f, 5._f, 5._f));

    storage.init();

    REQUIRE(storage.getAll<Float>(QuantityIds::POSITIONS)[2] == makeArray(0._f, 0._f, 0._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::POSITIONS)[1] == makeArray(5._f, 5._f, 5._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::POSITIONS)[0] == makeArray(5._f, 5._f, 5._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::MASSES)[1] == makeArray(0._f, 0._f, 0._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::MASSES)[0] == makeArray(5._f, 5._f, 5._f));
    REQUIRE(storage.getAll<Float>(QuantityIds::DENSITY)[0] == makeArray(5._f, 5._f, 5._f));
}

TEST_CASE("Storage material", "[storage]") {
    Settings<BodySettingsIds> settings;
    settings.set(BodySettingsIds::ADIABATIC_INDEX, 5._f);
    settings.set(BodySettingsIds::SHEAR_MODULUS, 2._f);
    settings.set(BodySettingsIds::ELASTICITY_LIMIT, 3._f);
    settings.set<EosEnum>(BodySettingsIds::EOS, EosEnum::IDEAL_GAS);

    Storage storage(settings);
    storage.insert<Vector, OrderEnum::SECOND_ORDER>(
        QuantityIds::POSITIONS, makeArray(Vector(1._f, 0._f, 0._f), Vector(-2._f, 1._f, 1._f)));
    /*Material& mat = storage.getMaterial(0);
    REQUIRE(mat.adiabaticIndex == 5._f);
    REQUIRE(mat.shearModulus == 2._f);
    REQUIRE(mat.elasticityLimit == 3._f);*/

    settings.set<Float>(BodySettingsIds::ADIABATIC_INDEX, 13._f);
    Storage other(settings);
    other.insert<Vector, OrderEnum::SECOND_ORDER>(
        QuantityIds::POSITIONS, makeArray(Vector(-3._f, 4._f, 0._f), Vector(5._f, 1._f, 0._f)));

    storage.merge(std::move(other));
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::DENSITY, 1._f);
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::ENERGY, 1._f);
    auto pressure = [&](const int i) { return EosAccessor(storage).evaluate(i).get<0>(); };
    REQUIRE(pressure(0) == 4._f);
    REQUIRE(pressure(1) == 4._f);
    REQUIRE(pressure(2) == 12._f);
    REQUIRE(pressure(3) == 12._f);

    Array<Material> mats;
    Material m1;
    m1.eos = Factory::getEos(settings);
    mats.push(std::move(m1));

    settings.set<Float>(BodySettingsIds::ADIABATIC_INDEX, 25._f);
    Material m2;
    m2.eos = Factory::getEos(settings);
    mats.push(std::move(m2));

    /*storage.setMaterial(std::move(mats), [](const Vector& pos, int) {
        if (pos[X] > 0._f) {
            return 0;
        } else {
            return 1;
        }
    });
    REQUIRE(pressure(0) == 12._f);
    REQUIRE(pressure(1) == 24._f);
    REQUIRE(pressure(2) == 24._f);
    REQUIRE(pressure(3) == 12._f);*/
}

TEST_CASE("Storage removeAll", "[storage]") {
    Storage storage;
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::MATERIAL_IDX, Array<Float>{ 0 }); // dummy unit
    storage.resize(3);
    storage.insert<Float, OrderEnum::SECOND_ORDER>(QuantityIds::POSITIONS, 3._f);
    storage.insert<Float, OrderEnum::FIRST_ORDER>(QuantityIds::MASSES, 1._f);
    storage.insert<Float, OrderEnum::ZERO_ORDER>(QuantityIds::DENSITY, 2._f);

    storage.removeAll();
    REQUIRE(storage.getParticleCnt() == 0);
    REQUIRE(storage.getQuantityCnt() == 0);
    storage.insert<Float, OrderEnum::SECOND_ORDER>(QuantityIds::POSITIONS, { 3._f, 2._f, 5._f });
    REQUIRE(storage.getParticleCnt() == 3);
    REQUIRE(storage.getQuantityCnt() == 1);
}
