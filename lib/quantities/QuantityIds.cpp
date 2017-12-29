#include "quantities/QuantityIds.h"
#include "common/Assert.h"

NAMESPACE_SPH_BEGIN

QuantityMetadata::QuantityMetadata(const std::string& fullName,
    const std::wstring& label,
    const ValueEnum type,
    const std::string& dtName,
    const std::string& d2tName)
    : quantityName(fullName)
    , label(label)
    , expectedType(type) {
    if (!dtName.empty()) {
        derivativeName = dtName;
    } else {
        derivativeName = quantityName + " derivative";
    }

    if (!d2tName.empty()) {
        secondDerivativeName = d2tName;
    } else {
        secondDerivativeName = quantityName + " 2nd derivative";
    }
}

QuantityMetadata getMetadata(const QuantityId key) {
    switch (key) {
    case QuantityId::POSITION:
        return QuantityMetadata("Position", L"r", ValueEnum::VECTOR, "Velocity", "Acceleration");
    case QuantityId::MASS:
        return QuantityMetadata("Particle mass", L"m", ValueEnum::SCALAR);
    case QuantityId::PRESSURE:
        return QuantityMetadata("Pressure", L"p", ValueEnum::SCALAR);
    case QuantityId::DENSITY:
        return QuantityMetadata("Density", L"\u03C1" /*rho*/, ValueEnum::SCALAR);
    case QuantityId::ENERGY:
        return QuantityMetadata("Specific energy", L"u", ValueEnum::SCALAR);
    case QuantityId::SOUND_SPEED:
        return QuantityMetadata("Sound speed", L"c_s", ValueEnum::SCALAR);
    case QuantityId::DEVIATORIC_STRESS:
        return QuantityMetadata("Deviatoric stress", L"S", ValueEnum::TRACELESS_TENSOR);
    case QuantityId::SPECIFIC_ENTROPY:
        return QuantityMetadata("Specific entropy", L"s", ValueEnum::SCALAR);
    case QuantityId::ENERGY_DENSITY:
        return QuantityMetadata("Energy density", L"q", ValueEnum::SCALAR);
    case QuantityId::ENERGY_PER_PARTICLE:
        return QuantityMetadata("Energy per particle", L"U", ValueEnum::SCALAR);
    case QuantityId::DAMAGE:
        return QuantityMetadata("Damage", L"D", ValueEnum::SCALAR);
    case QuantityId::EPS_MIN:
        return QuantityMetadata("Activation strain", L"\u03B5" /*epsilon*/, ValueEnum::SCALAR);
    case QuantityId::M_ZERO:
        return QuantityMetadata("Weibull exponent of stretched distribution", L"m_0", ValueEnum::SCALAR);
    case QuantityId::EXPLICIT_GROWTH:
        return QuantityMetadata("Explicit crack growth", L"???", ValueEnum::SCALAR);
    case QuantityId::N_FLAWS:
        return QuantityMetadata("Number of flaws", L"N_flaws", ValueEnum::INDEX);
    case QuantityId::FLAW_ACTIVATION_IDX:
        return QuantityMetadata("Flaw activation idx", L"Act", ValueEnum::INDEX);
    case QuantityId::STRESS_REDUCING:
        return QuantityMetadata("Yielding reduce", L"Red", ValueEnum::SCALAR);
    case QuantityId::VELOCITY_GRADIENT:
        return QuantityMetadata("Velocity gradient", L"\u2207v" /*nabla v*/, ValueEnum::SYMMETRIC_TENSOR);
    case QuantityId::VELOCITY_DIVERGENCE:
        return QuantityMetadata("Velocity divergence", L"\u2207\u22C5v" /*nabla cdot v*/, ValueEnum::SCALAR);
    case QuantityId::VELOCITY_ROTATION:
        return QuantityMetadata("Velocity rotation", L"\u2207\u2A2Fv" /*nabla cross v*/, ValueEnum::VECTOR);
    case QuantityId::STRENGTH_VELOCITY_GRADIENT:
        return QuantityMetadata("Strength velocity gradient", L"\u2207v", ValueEnum::SYMMETRIC_TENSOR);
    case QuantityId::ANGULAR_MOMENTUM_CORRECTION:
        return QuantityMetadata("Correction tensor", L"C_ij", ValueEnum::SYMMETRIC_TENSOR);
    case QuantityId::AV_ALPHA:
        return QuantityMetadata("AV alpha", L"\u03B1_AV" /*alpha_AV*/, ValueEnum::SCALAR);
    case QuantityId::AV_BETA:
        return QuantityMetadata("AV beta", L"\u03B2_AV" /*beta_AV*/, ValueEnum::SCALAR);
    case QuantityId::AV_STRESS:
        return QuantityMetadata("Artificial stress", L"R", ValueEnum::SYMMETRIC_TENSOR);
    case QuantityId::INTERPARTICLE_SPACING_KERNEL:
        return QuantityMetadata("Interparticle spacing kernel", L"w(\u0394 p)", ValueEnum::SCALAR);
    case QuantityId::DISPLACEMENT:
        return QuantityMetadata("Displacement", L"u", ValueEnum::VECTOR);
    case QuantityId::FLAG:
        return QuantityMetadata("Flag", L"flag", ValueEnum::INDEX);
    case QuantityId::MATERIAL_ID:
        return QuantityMetadata("MaterialId", L"matID", ValueEnum::INDEX);
    case QuantityId::XSPH_VELOCITIES:
        return QuantityMetadata("XSPH correction", L"v_xsph", ValueEnum::VECTOR);
    case QuantityId::GRAD_H:
        return QuantityMetadata("Grad-h terms", L"\u03A9" /*Omega*/, ValueEnum::SCALAR);
    case QuantityId::GRAVITY_POTENTIAL:
        return QuantityMetadata("Grav. potential", L"\u03A6" /*Phi*/, ValueEnum::SCALAR);
    case QuantityId::MOVEMENT_TIME:
        return QuantityMetadata("Movement time", L"dt", ValueEnum::SCALAR);
    case QuantityId::ANGULAR_VELOCITY:
        return QuantityMetadata("Angular velocity", L"\u03C9" /*omega*/, ValueEnum::VECTOR);
    case QuantityId::NEIGHBOUR_CNT:
        return QuantityMetadata("Neigh. cnt", L"N_neigh", ValueEnum::INDEX);
    case QuantityId::SURFACE_NORMAL:
        return QuantityMetadata("Surf. normal", L"n", ValueEnum::VECTOR);
    case QuantityId::MOMENT_OF_INERTIA:
        return QuantityMetadata("Mom. of intertia", L"I", ValueEnum::SCALAR);
    case QuantityId::PHASE_ANGLE:
        return QuantityMetadata("Phase angle", L"\u03C6" /*phi*/, ValueEnum::VECTOR);
    default:
        NOT_IMPLEMENTED;
    }
}

NAMESPACE_SPH_END
