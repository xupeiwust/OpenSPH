#pragma once

#include "system/Timer.h"
#include <map>
#include <memory>

NAMESPACE_SPH_BEGIN


/// Timer that reports the measured duration when being destroyed
struct ScopedTimer : public Object {
private:
    StoppableTimer impl;
    std::string name;

    using OnScopeEnds = std::function<void(const std::string&, const uint64_t)>;
    OnScopeEnds callback;

public:
    ScopedTimer(const std::string& name, const OnScopeEnds& callback)
        : name(name)
        , callback(callback) {}

    ~ScopedTimer() { callback(name, impl.elapsed<TimerUnit::MICROSECOND>()); }

    void stop() { impl.stop(); }

    void resume() { impl.resume(); }
};

struct ScopeStatistics {
    std::string name;
    uint64_t totalTime; // time spent in function (in ms)
    float relativeTime;
};

namespace Abstract {
    class Logger;
}

/// Profiler object implemented as singleton.
class Profiler : public Noncopyable {
private:
    static std::unique_ptr<Profiler> instance;

    struct Duration {
        uint64_t time = 0; // initialize to zerp
    };
    // map of profiled scopes, its key being a string = name of the scope
    std::map<std::string, Duration> map;


public:
    static Profiler* getInstance() {
        if (!instance) {
            instance = std::make_unique<Profiler>();
        }
        return instance.get();
    }

    /// Creates a new scoped timer of given name. The timer will automatically adds elapsed time to the
    /// profile when being destroyed.
    ScopedTimer makeScopedTimer(const std::string& name) {
        return ScopedTimer(name, [this](const std::string& n, const uint64_t elapsed) {
            ASSERT(elapsed > 0 && "too small scope to be measured");
            map[n].time += elapsed;
        });
    }

    /// Returns the array of scope statistics, sorted by elapsed time.
    Array<ScopeStatistics> getStatistics() const;

    /// Prints statistics into the logger.
    void printStatistics(Abstract::Logger* logger) const;

    /// Clears all records, mainly for testing purposes
    void clear() { map.clear(); }
};

#ifdef PROFILE
#define PROFILE_SCOPE(name)                                                                                  \
    Profiler* __instance      = Profiler::getInstance();                                                     \
    ScopedTimer __scopedTimer = __instance->makeScopedTimer(name);
#define SCOPE_STOP __scopedTimer.stop()
#define SCOPE_RESUME __scopedTimer.resume()
#else
#define PROFILE_SCOPE(name)
#define SCOPE_STOP
#define SCOPE_RESUME
#endif

NAMESPACE_SPH_END
