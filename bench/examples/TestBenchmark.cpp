#include "bench/Session.h"

using namespace Sph;

BENCHMARK("Test benchmark", "[testgroup]", Benchmark::Context& context) {
    while (context.running()) {
        Size sum = 0;
        for (Size i = 0; i < 10000; ++i) {
            sum += 10000;
        }
    }
}
