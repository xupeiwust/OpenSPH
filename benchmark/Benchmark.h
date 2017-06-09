#pragma once

#include "common/ForwardDecl.h"
#include "objects/containers/Array.h"
#include "objects/wrappers/Outcome.h"
#include "objects/wrappers/Range.h"
#include "objects/wrappers/SharedPtr.h"
#include "system/Timer.h"
#include <functional>

#define NAMESPACE_BENCHMARK_BEGIN NAMESPACE_SPH_BEGIN namespace Benchmark {
#define NAMESPACE_BENCHMARK_END                                                                              \
    }                                                                                                        \
    NAMESPACE_SPH_END


NAMESPACE_BENCHMARK_BEGIN

class Stats {
private:
    Float sum = 0._f;
    Float sumSqr = 0._f;
    Size cnt = 0;
    Range range;

public:
    INLINE void add(const Float value) {
        sum += value;
        sumSqr += sqr(value);
        range.extend(value);
        cnt++;
    }

    INLINE Float mean() const {
        ASSERT(cnt != 0);
        return sum / cnt;
    }

    INLINE Float variance() const {
        if (cnt < 2) {
            return INFTY;
        }
        const Float cntInv = 1._f / cnt;
        return cntInv * (sumSqr * cntInv - sqr(sum * cntInv));
    }

    INLINE Size count() const {
        return cnt;
    }

    INLINE Float min() const {
        return range.lower();
    }

    INLINE Float max() const {
        return range.upper();
    }
};

/// Specifies the ending condition of the benchmark
struct LimitCondition {
    enum class Type {
        VARIANCE,  ///< Mean variance of iteration gets below the threshold
        TIME,      ///< Benchmark ran for given duration
        ITERATION, ///< Benchmark performed given number of iteration
    };

    Type type = Type::VARIANCE;
    Float variance = 0.02_f; // 2%
    Size duration = 500;     // 500ms
    Size iterationCnt = 100;
};


class Context {
private:
    bool state = true;

    Size iterationCnt = 0;

    LimitCondition limit;

    Stats& stats;

    Timer iterationTimer;

    Timer totalTimer;

    /// Name of the running benchmark
    std::string& name;

public:
    Context(const LimitCondition limit, Stats& stats, std::string& name)
        : limit(limit)
        , stats(stats)
        , name(name) {}

    /// Whether to keep running or exit
    INLINE bool running() {
        state = this->shouldContinue();
        iterationCnt++;
        stats.add(iterationTimer.elapsed(TimerUnit::MILLISECOND));
        iterationTimer.restart();
        return state;
    }

    INLINE Size elapsed() const {
        return totalTimer.elapsed(TimerUnit::MILLISECOND);
    }

private:
    INLINE bool shouldContinue() const {
        switch (limit.type) {
        case LimitCondition::Type::ITERATION:
            return stats.count() < limit.iterationCnt;
        case LimitCondition::Type::TIME:
            return totalTimer.elapsed(TimerUnit::MILLISECOND) < limit.duration;
        case LimitCondition::Type::VARIANCE:
            return stats.variance() < limit.variance;
        default:
            NOT_IMPLEMENTED;
        }
    }
};

/// Single benchmark unit
class Unit {
private:
    std::string name;

    std::function<void(Context&)> func;

public:
    Unit(const std::string& name, std::function<void(Context&)> func)
        : name(name)
        , func(std::move(func)) {
        ASSERT(func != nullptr);
    }

    const std::string& getName() const {
        return name;
    }

    Outcome run(Stats& stats, Size& elapsed) {
        Context context(LimitCondition(), stats, name);
        func(context);
        elapsed = context.elapsed();
        return SUCCESS;
    }
};

class Group {
private:
    std::string name;
    Array<SharedPtr<Unit>> benchmarks;

public:
    Group() = default;

    Group(const std::string& name)
        : name(name) {}

    const std::string& getName() const {
        return name;
    }

    void addBenchmark(const SharedPtr<Unit>& benchmark) {
        benchmarks.push(benchmark);
    }
};

class Session {
private:
    /// Global instance of the session
    static AutoPtr<Session> instance;

    /// List of all benchmarks in the session
    Array<SharedPtr<Unit>> benchmarks;

    /// Benchmark groups
    Array<Group> groups;

    /// Logger used to output benchmark results
    AutoPtr<Abstract::Logger> logger;

    /// Status of the session, contains an error if the session is in invalid state.
    Outcome status = SUCCESS;

    enum class Flag {
        COMPARE_WITH_BASELINE = 1 << 0, ///< Compare results with baseline

        MAKE_BASELINE = 1 << 1, ///< Record and cache baseline

        SILENT = 1 << 2, ///< Only print failed benchmarks
    };

    struct {
        /// Run only selected group of benchmarks
        std::string group;

        Flags<Flag> flags;
    } params;

public:
    Session();

    static Session& getInstance();

    /// Adds a new benchmark into the session.
    void registerBenchmark(const SharedPtr<Unit>& benchmark, const std::string& groupName);

    /// Runs all benchmarks.
    Outcome run(int argc, char* argv[]);

    ~Session();

private:
    Group& getGroupByName(const std::string& groupName);

    Outcome parseArgs(int arcs, char* argv[]);

    void log(const std::string& text);
};

class Register {
public:
    Register(const SharedPtr<Unit>& benchmark, const std::string& groupName);
};

#define BENCHMARK_UNIQUE_NAME(prefix) prefix##__LINE__

#define BENCHMARK_FUNCTION_NAME BENCHMARK_UNIQUE_NAME(BENCHMARK)

#define BENCHMARK_REGISTER_NAME BENCHMARK_UNIQUE_NAME(REGISTER)

#define BENCHMARK(name, group, ...)                                                                          \
    void BENCHMARK_FUNCTION_NAME(__VA_ARGS__);                                                               \
    ::Sph::Benchmark::Register BENCHMARK_REGISTER_NAME(                                                      \
        makeShared<::Sph::Benchmark::Unit>(name, &BENCHMARK_FUNCTION_NAME), group);                          \
    void BENCHMARK_FUNCTION_NAME(__VA_ARGS__)

NAMESPACE_BENCHMARK_END
