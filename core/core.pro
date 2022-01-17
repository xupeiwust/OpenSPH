TEMPLATE = lib
CONFIG += c++14 staticlib thread silent
CONFIG -= app_bundle qt

include(sharedCore.pro)

SOURCES += \
    common/Assert.cpp \
    gravity/AggregateSolver.cpp \
    gravity/BarnesHut.cpp \
    gravity/NBodySolver.cpp \
    io/FileManager.cpp \
    io/FileSystem.cpp \
    io/Logger.cpp \
    io/Output.cpp \
    io/Path.cpp \
    io/Vdb.cpp \
    math/Curve.cpp \
    math/Morton.cpp \
    math/SparseMatrix.cpp \
    math/rng/Rng.cpp \
    objects/containers/String.cpp \
    objects/finders/HashMapFinder.cpp \
    objects/finders/IncrementalFinder.cpp \
    objects/finders/KdTree.cpp \
    objects/finders/NeighborFinder.cpp \
    objects/finders/PeriodicFinder.cpp \
    objects/finders/UniformGrid.cpp \
    objects/geometry/Delaunay.cpp \
    objects/geometry/Domain.cpp \
    objects/geometry/SymmetricTensor.cpp \
    objects/utility/Dynamic.cpp \
    objects/utility/Streams.cpp \
    physics/Damage.cpp \
    physics/Eos.cpp \
    physics/Functions.cpp \
    physics/Integrals.cpp \
    physics/Rheology.cpp \
    physics/TimeFormat.cpp \
    physics/Units.cpp \
    post/Analysis.cpp \
    post/TwoBody.cpp \
    post/Compare.cpp \
    post/MarchingCubes.cpp \
    post/Mesh.cpp \
    post/MeshFile.cpp \
    post/Plot.cpp \
    post/StatisticTests.cpp \
    quantities/Attractor.cpp \
    quantities/IMaterial.cpp \
    quantities/Particle.cpp \
    quantities/Quantity.cpp \
    quantities/QuantityIds.cpp \
    quantities/Storage.cpp \
    quantities/Utility.cpp \
    run/IRun.cpp \
    run/Job.cpp \
    run/Node.cpp \
    run/ScriptNode.cpp \
    run/ScriptUtils.cpp \
    run/SpecialEntries.cpp \
    run/VirtualSettings.cpp \
    run/jobs/GeometryJobs.cpp \
    run/jobs/InitialConditionJobs.cpp \
    run/jobs/IoJobs.cpp \
    run/jobs/MaterialJobs.cpp \
    run/jobs/ParticleJobs.cpp \
    run/jobs/ScriptJobs.cpp \
    run/jobs/SimulationJobs.cpp \
    sph/Diagnostics.cpp \
    sph/Materials.cpp \
    sph/boundary/Boundary.cpp \
    sph/equations/Accumulated.cpp \
    sph/equations/Derivative.cpp \
    sph/equations/EquationTerm.cpp \
    sph/equations/Rotation.cpp \
    sph/equations/av/Stress.cpp \
    sph/initial/Distribution.cpp \
    sph/initial/Initial.cpp \
    sph/initial/MeshDomain.cpp \
    sph/initial/Galaxy.cpp \
    sph/initial/Stellar.cpp \
    sph/initial/UvMapping.cpp \
    sph/solvers/AsymmetricSolver.cpp \
    sph/solvers/EnergyConservingSolver.cpp \
    sph/solvers/EquilibriumSolver.cpp \
    sph/solvers/GradHSolver.cpp \
    sph/solvers/GravitySolver.cpp \
    sph/solvers/StandardSets.cpp \
    sph/solvers/SummationSolver.cpp \
    sph/solvers/SymmetricSolver.cpp \
    system/ArgsParser.cpp \
    system/Factory.cpp \
    system/Platform.cpp \
    system/Process.cpp \
    system/Profiler.cpp \
    system/Settings.cpp \
    system/Statistics.cpp \
    system/Timer.cpp \
    tests/Setup.cpp \
    thread/CheckFunction.cpp \
    thread/OpenMp.cpp \
    thread/Pool.cpp \
    thread/Scheduler.cpp \
    thread/Tbb.cpp \
    timestepping/TimeStepCriterion.cpp \
    timestepping/TimeStepping.cpp \
    io/LogWriter.cpp \
    sph/solvers/ElasticDeformationSolver.cpp \
    sph/solvers/DensityIndependentSolver.cpp \
    run/Config.cpp \
    run/jobs/Presets.cpp

HEADERS += \
    Sph.h \
    common/Assert.h \
    common/ForwardDecl.h \
    common/Globals.h \
    common/Traits.h \
    gravity/AggregateSolver.h \
    gravity/BarnesHut.h \
    gravity/BruteForceGravity.h \
    gravity/CachedGravity.h \
    gravity/Collision.h \
    gravity/IGravity.h \
    gravity/Moments.h \
    gravity/NBodySolver.h \
    gravity/SphericalGravity.h \
    gravity/SymmetricGravity.h \
    io/Column.h \
    io/FileManager.h \
    io/FileSystem.h \
    io/Logger.h \
    io/Output.h \
    io/Path.h \
    io/Serializer.h \
    io/Table.h \
    io/Vdb.h \
    math/AffineMatrix.h \
    math/Curve.h \
    math/Functional.h \
    math/MathBasic.h \
    math/MathUtils.h \
    math/Matrix.h \
    math/Means.h \
    math/Morton.h \
    math/Quat.h \
    math/SparseMatrix.h \
    math/rng/Rng.h \
    math/rng/VectorRng.h \
    objects/containers/AdvancedAllocators.h \
    objects/containers/BasicAllocators.h \
    objects/containers/CircularArray.h \
    objects/containers/Tags.h \
    objects/finders/IncrementalFinder.h \
    objects/finders/NeighborFinder.h \
    objects/geometry/Delaunay.h \
    objects/geometry/Plane.h \
    objects/utility/OutputIterators.h \
    objects/utility/Progressible.h \
    objects/utility/Streams.h \
    objects/wrappers/ExtendedEnum.h \
    objects/wrappers/MultiLambda.h \
    objects/wrappers/SharedToken.h \
    quantities/Attractor.h \
    quantities/Utility.h \
    run/Job.inl.h \
    sph/initial/UvMapping.h \
    objects/Exceptions.h \
    objects/Object.h \
    objects/containers/Array.h \
    objects/containers/ArrayRef.h \
    objects/containers/ArrayView.h \
    objects/containers/CallbackSet.h \
    objects/containers/FlatMap.h \
    objects/containers/FlatSet.h \
    objects/containers/UnorderedMap.h \
    objects/containers/Grid.h \
    objects/containers/List.h \
    objects/containers/LookupMap.h \
    objects/containers/Queue.h \
    objects/containers/StaticArray.h \
    objects/containers/String.h \
    objects/containers/Tuple.h \
    objects/containers/Volume.h \
    objects/finders/AdaptiveGrid.h \
    objects/finders/BruteForceFinder.h \
    objects/finders/Bvh.h \
    objects/finders/Bvh.inl.h \
    objects/finders/HashMapFinder.h \
    objects/finders/KdTree.h \
    objects/finders/KdTree.inl.h \
    objects/finders/Linkedlist.h \
    objects/finders/Octree.h \
    objects/finders/Order.h \
    objects/finders/UniformGrid.h \
    objects/geometry/AntisymmetricTensor.h \
    objects/geometry/Box.h \
    objects/geometry/Domagin.h \
    objects/geometry/Domain.h \
    objects/geometry/Generic.h \
    objects/geometry/Indices.h \
    objects/geometry/Multipole.h \
    objects/geometry/Sphere.h \
    objects/geometry/SymmetricTensor.h \
    objects/geometry/Tensor.h \
    objects/geometry/TracelessTensor.h \
    objects/geometry/Triangle.h \
    objects/geometry/Vector.h \
    objects/utility/Algorithm.h \
    objects/utility/Dynamic.h \
    objects/utility/EnumMap.h \
    objects/utility/Iterator.h \
    objects/utility/IteratorAdapters.h \
    objects/utility/OperatorTemplate.h \
    objects/utility/PerElementWrapper.h \
    objects/wrappers/AlignedStorage.h \
    objects/wrappers/Any.h \
    objects/wrappers/AutoPtr.h \
    objects/wrappers/ClonePtr.h \
    objects/wrappers/Expected.h \
    objects/wrappers/Finally.h \
    objects/wrappers/Flags.h \
    objects/wrappers/Function.h \
    objects/wrappers/Interval.h \
    objects/wrappers/Locking.h \
    objects/wrappers/LockingPtr.h \
    objects/wrappers/Lut.h \
    objects/wrappers/NonOwningPtr.h \
    objects/wrappers/ObserverPtr.h \
    objects/wrappers/Optional.h \
    objects/wrappers/Outcome.h \
    objects/wrappers/PropagateConst.h \
    objects/wrappers/RawPtr.h \
    objects/wrappers/SharedPtr.h \
    objects/wrappers/Variant.h \
    physics/Constants.h \
    physics/Damage.h \
    physics/Eos.h \
    physics/Functions.h \
    physics/Integrals.h \
    physics/Rheology.h \
    physics/TimeFormat.h \
    physics/Units.h \
    post/Analysis.h \
    post/TwoBody.h \
    post/Compare.h \
    post/MarchingCubes.h \
    post/Mesh.h \
    post/MeshFile.h \
    post/Plot.h \
    post/Point.h \
    post/StatisticTests.h \
    quantities/IMaterial.h \
    quantities/Iterate.h \
    quantities/Particle.h \
    quantities/Quantity.h \
    quantities/QuantityHelpers.h \
    quantities/QuantityIds.h \
    quantities/Storage.h \
    run/IRun.h \
    run/Job.h \
    run/Node.h \
    run/ScriptNode.h \
    run/ScriptUtils.h \
    run/Trigger.h \
    run/jobs/GeometryJobs.h \
    run/jobs/InitialConditionJobs.h \
    run/jobs/IoJobs.h \
    run/jobs/MaterialJobs.h \
    run/jobs/ParticleJobs.h \
    run/jobs/ScriptJobs.h \
    run/jobs/SimulationJobs.h \
    sph/Diagnostics.h \
    sph/Materials.h \
    sph/boundary/Boundary.h \
    sph/equations/Accumulated.h \
    sph/equations/DeltaSph.h \
    sph/equations/Derivative.h \
    sph/equations/DerivativeHelpers.h \
    sph/equations/EquationTerm.h \
    sph/equations/Fluids.h \
    sph/equations/Friction.h \
    sph/equations/Heat.h \
    sph/equations/HelperTerms.h \
    sph/equations/Potentials.h \
    sph/equations/Rotation.h \
    sph/equations/XSph.h \
    sph/equations/Yorp.h \
    sph/equations/av/Balsara.h \
    sph/equations/av/Conductivity.h \
    sph/equations/av/MorrisMonaghan.h \
    sph/equations/av/Riemann.h \
    sph/equations/av/Standard.h \
    sph/equations/av/Stress.h \
    sph/equations/heat/Heat.h \
    sph/equationsav/Standard.h \
    sph/initial/Distribution.h \
    sph/initial/Initial.h \
    sph/initial/MeshDomain.h \
    sph/initial/Stellar.h \
    sph/initial/Galaxy.h \
    sph/kernel/GravityKernel.h \
    sph/kernel/Interpolation.h \
    sph/kernel/Kernel.h \
    sph/solvers/AsymmetricSolver.h \
    sph/solvers/CollisionSolver.h \
    sph/solvers/DensityIndependentSolver.h \
    sph/solvers/EnergyConservingSolver.h \
    sph/solvers/EntropySolver.h \
    sph/solvers/EquilibriumSolver.h \
    sph/solvers/GradHSolver.h \
    sph/solvers/GravitySolver.h \
    sph/solvers/SimpleSolver.h \
    sph/solvers/StabilizationSolver.h \
    sph/solvers/StandardSets.h \
    sph/solvers/SummationSolver.h \
    sph/solvers/SymmetricSolver.h \
    system/ArgsParser.h \
    system/ArrayStats.h \
    system/Column.h \
    system/Crashpad.h \
    system/Element.h \
    system/Factory.h \
    system/Platform.h \
    system/Process.h \
    system/Profiler.h \
    system/ScriptUtils.h \
    system/Settings.h \
    system/Settings.impl.h \
    system/Statistics.h \
    system/Timer.h \
    tests/Approx.h \
    tests/Setup.h \
    thread/AtomicFloat.h \
    thread/CheckFunction.h \
    thread/OpenMp.h \
    thread/Pool.h \
    thread/Scheduler.h \
    thread/Tbb.h \
    thread/ThreadLocal.h \
    timestepping/ISolver.h \
    timestepping/ISolverr.h \
    timestepping/TimeStepCriterion.h \
    timestepping/TimeStepping.h \
    io/LogWriter.h \
    objects/finders/PeriodicFinder.h \
    sph/solvers/ElasticDeformationSolver.h \
    run/VirtualSettings.h \
    run/VirtualSettings.inl.h \
    run/Config.h \
    run/jobs/Presets.h \
    run/jobs/SpecialEntries.h \
    run/SpecialEntries.h
