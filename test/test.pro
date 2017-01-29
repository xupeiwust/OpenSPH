TEMPLATE = app
CONFIG += c++14 thread silent
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -msse4.1 -Wall -Wextra -Werror -std=c++14 -pthread

#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -Os
#QMAKE_CXXFLAGS_DEBUG += -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error
QMAKE_CXX = g++

CONFIG(release, debug|profile|assert|release) {
  message( "SPH TESTS --- Building for Release" )
}

CONFIG(profile, debug|profile|assert|release) {
  message( "SPH TESTS --- Building for Profile" )
  DEFINES += PROFILE
}

CONFIG(assert, debug|profile|assert|release) {
  message( "SPH TESTS --- Building for Assert" )
  DEFINES += DEBUG PROFILE
  QMAKE_CXXFLAGS += -O2
}

CONFIG(debug, debug|profile|assert|release) {
  message( "SPH TESTS --- Building for Debug" )
  DEFINES += DEBUG PROFILE
}


INCLUDEPATH += ../lib ../../external/Catch/include
DEPENDPATH += . ../lib
LIBS += ../lib/libsph.a

SOURCES += \
    main.cpp \
    utils/Utils.cpp \
    ../lib/math/test/Integrator.cpp \
    ../lib/math/test/Math.cpp \
    ../lib/math/rng/test/Rng.cpp \
    ../lib/math/rng/test/VectorRng.cpp \
    ../lib/math/test/Morton.cpp \
    ../lib/physics/test/TimeFormat.cpp \
    ../lib/physics/test/Eos.cpp \
    ../lib/physics/test/Integrals.cpp \
    ../lib/physics/test/Damage.cpp \
    ../lib/geometry/test/Vector.cpp \
    ../lib/geometry/test/Indices.cpp \
    ../lib/geometry/test/Multipole.cpp \
    ../lib/geometry/test/TracelessTensor.cpp \
    ../lib/geometry/test/Tensor.cpp \
    ../lib/objects/wrappers/test/Optional.cpp \
    ../lib/objects/wrappers/test/Variant.cpp \
    ../lib/objects/wrappers/test/Iterators.cpp \
    ../lib/objects/wrappers/test/Range.cpp \
    ../lib/objects/wrappers/test/NonOwningPtr.cpp \
    ../lib/objects/wrappers/test/Any.cpp \
    ../lib/objects/wrappers/test/VectorizedArray.cpp \
    ../lib/objects/wrappers/test/AlignedStorage.cpp \
    ../lib/objects/wrappers/test/Flags.cpp \
    ../lib/objects/containers/test/Array.cpp \
    ../lib/objects/containers/test/ArrayUtils.cpp \
    ../lib/objects/containers/test/BufferedArray.cpp \
    ../lib/objects/containers/test/Tuple.cpp \
    ../lib/objects/containers/test/StaticArray.cpp \
    ../lib/objects/finders/test/Order.cpp \
    ../lib/objects/finders/test/BruteForce.cpp \
    ../lib/objects/finders/test/Finders.cpp \
    ../lib/sph/timestepping/test/TimeStepping.cpp \
    ../lib/sph/timestepping/test/TimeStepCriterion.cpp \
    ../lib/sph/initial/test/Initial.cpp \
    ../lib/sph/initial/test/Distribution.cpp \
    ../lib/sph/kernel/test/Kernel.cpp \
    ../lib/sph/av/test/Balsara.cpp \
    ../lib/sph/boundary/test/Boundary.cpp \
    ../lib/system/test/Settings.cpp \
    ../lib/system/test/Timer.cpp \
    ../lib/system/test/Profiler.cpp \
    ../lib/system/test/Output.cpp \
    ../lib/system/test/CallbackList.cpp \
    ../lib/system/test/ArrayStats.cpp \
    ../lib/system/test/Statistics.cpp \
    ../lib/quantities/test/Quantity.cpp \
    ../lib/quantities/test/Storage.cpp \
    ../lib/quantities/test/Iterate.cpp \
    ../lib/quantities/test/QuantityHelpers.cpp \
    ../lib/solvers/test/ContinuitySolver.cpp \
    ../lib/solvers/test/Module.cpp \
    ../lib/post/test/Components.cpp \
    ../lib/physics/test/Yielding.cpp \
    ../lib/quantities/test/Material.cpp \
    ../lib/solvers/test/Accumulator.cpp \
    ../lib/geometry/test/Domain.cpp \
    ../lib/thread/test/Pool.cpp \
    utils/Approx.cpp \
    utils/SequenceTest.cpp \
    ../lib/geometry/test/Box.cpp


HEADERS += \
    utils/Utils.h \
    utils/RecordType.h \
    ../lib/sph/forces/StressForce.h \
    utils/Approx.h \
    utils/SequenceTest.h
