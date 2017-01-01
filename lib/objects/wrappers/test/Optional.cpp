#include "objects/wrappers/Optional.h"
#include "objects/containers/Array.h"
#include "catch.hpp"
#include "utils/RecordType.h"

using namespace Sph;


TEST_CASE("Optional constructor", "[optional]") {
    Optional<RecordType> o1;
    REQUIRE(!o1);
    o1 = RecordType(5);
    REQUIRE(o1);
    REQUIRE(o1.get().value == 5);
    REQUIRE(o1.get().wasMoveConstructed);

    Optional<RecordType> o2(o1);
    REQUIRE(o2);
    REQUIRE(o2.get().value == 5);

    Optional<int> o3;
    o3.emplace(2);
    REQUIRE(o3);
    REQUIRE(o3.get() == 2);
}

TEST_CASE("Optional Lvalue References", "[optional]") {
    Optional<int&> o1 (NOTHING);
    REQUIRE(!o1);
    Optional<int&> o2(o1);
    REQUIRE(!o2);
    int value = 5;
    Optional<int&> o3(value);
    REQUIRE(o3);
    REQUIRE(o3.get() == 5);
    o3.get() = 3;
    REQUIRE(value == 3);

    Optional<const int&> o4(value);
    REQUIRE(o4);
    REQUIRE(o4.get() == 3);

    // turn off
    o3 = o1;
    REQUIRE(!o3);
}

/*TEST_CASE("Optional Rvalue References",  "[optional]") {
    Optional<RecordType&&> o1(NOTHING);
    REQUIRE(!o1);
    RecordType r1(5);
    Optional<RecordType&&> o2(std::move(r1));
    REQUIRE(o2);
    REQUIRE(!o2.get().wasMoveConstructed);
    REQUIRE(!r1.wasMoved);
    REQUIRE(o2.get() == 5);
    RecordType r2 = o2.get();
    REQUIRE(r2.wasMoveConstructed);
    REQUIRE(r1.wasMoved);
}*/

TEST_CASE("Optional forward", "[optional]") {

}

TEST_CASE("Optional cast", "[optional") {

}
