#include "system/Statistics.h"
#include "catch.hpp"
#include "tests/Approx.h"

using namespace Sph;

TEST_CASE("Statistics set/get", "[statistics]") {
    Statistics stats;
    REQUIRE_FALSE(stats.has(StatisticsId::TIMESTEP_VALUE));
    stats.set(StatisticsId::TIMESTEP_VALUE, 5._f);
    REQUIRE(stats.has(StatisticsId::TIMESTEP_VALUE));
    REQUIRE(stats.get<Float>(StatisticsId::TIMESTEP_VALUE) == 5._f);

    stats.accumulate(StatisticsId::NEIGHBOR_COUNT, 2._f);
    stats.accumulate(StatisticsId::NEIGHBOR_COUNT, 7._f);
    stats.accumulate(StatisticsId::NEIGHBOR_COUNT, 6._f);
    REQUIRE(stats.has(StatisticsId::NEIGHBOR_COUNT));
    MinMaxMean f = stats.get<MinMaxMean>(StatisticsId::NEIGHBOR_COUNT);
    REQUIRE(f.max() == 7._f);
    REQUIRE(f.min() == 2._f);
    REQUIRE(f.mean() == approx(5._f));
    REQUIRE(f.count() == 3);
}
