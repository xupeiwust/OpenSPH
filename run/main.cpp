#include "physics/Eos.h"
#include "problem/Problem.h"
#include "sph/initial/Initial.h"
#include "system/Factory.h"
#include "system/Output.h"

using namespace Sph;

int main() {
    GlobalSettings globalSettings = GLOBAL_SETTINGS;
    globalSettings.set(GlobalSettingsIds::DOMAIN_TYPE, DomainEnum::SPHERICAL);
    globalSettings.set(GlobalSettingsIds::TIMESTEPPING_ADAPTIVE, true);
    globalSettings.set(GlobalSettingsIds::TIMESTEPPING_INITIAL_TIMESTEP, 1.e-6_f);
    globalSettings.set(GlobalSettingsIds::TIMESTEPPING_MAX_TIMESTEP, 1.e-1_f);
    globalSettings.set(GlobalSettingsIds::MODEL_FORCE_DIV_S, true);
    globalSettings.set(GlobalSettingsIds::SPH_FINDER, FinderEnum::VOXEL);
    globalSettings.set(GlobalSettingsIds::RUN_OUTPUT_STEP, 1);
    /*globalSettings.set(GlobalSettingsIds::MODEL_DAMAGE, DamageEnum::SCALAR_GRADY_KIPP);
    globalSettings.set(GlobalSettingsIds::MODEL_YIELDING, YieldingEnum::VON_MISES);*/
    Problem* p      = new Problem(globalSettings);
    p->logger       = std::make_unique<StdOutLogger>();
    p->timeRange    = Range(0._f, 1._f);
    p->timeStepping = Factory::getTimestepping(globalSettings, p->storage);
    p->output =
        std::make_unique<TextOutput>(globalSettings.get<std::string>(GlobalSettingsIds::RUN_OUTPUT_NAME),
                                     globalSettings.get<std::string>(GlobalSettingsIds::RUN_NAME),
                                     Array<QuantityKey>{ QuantityKey::POSITIONS, QuantityKey::DEVIATORIC_STRESS });

    auto bodySettings = BODY_SETTINGS;
    bodySettings.set(BodySettingsIds::ENERGY, 1.e2_f);
    bodySettings.set(BodySettingsIds::PARTICLE_COUNT, 10000);
    bodySettings.set(BodySettingsIds::EOS, EosEnum::TILLOTSON);
    InitialConditions conds(p->storage, globalSettings);

    SphericalDomain domain1(Vector(0._f), 5e2_f); // D = 1km
    conds.addBody(domain1, bodySettings);

    /*SphericalDomain domain2(Vector(6.e2_f, 1.35e2_f, 0._f), 20._f);
    bodySettings.set(BodySettingsIds::PARTICLE_COUNT, 100);
    conds.addBody(domain2, bodySettings, Vector(-5.e3_f, 0._f, 0._f));*/
    p->run();

    Profiler::getInstance()->printStatistics(p->logger.get());

    return 0;
}
