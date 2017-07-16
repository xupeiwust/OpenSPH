#include "gravity/BarnesHut.h"
#include "gravity/Moments.h"
#include "physics/Constants.h"

NAMESPACE_SPH_BEGIN

void BarnesHut::build(ArrayView<const Vector> points, ArrayView<const Float> masses) {
    // save source data
    r = points;
    m = masses;

    // build K-d Tree
    kdTree.build(r);

    if (SPH_UNLIKELY(r.empty())) {
        return;
    }
    // constructs nodes
    kdTree.iterate<KdTree::Direction::BOTTOM_UP>([this](KdNode& node, KdNode* left, KdNode* right) INL {
        if (node.isLeaf()) {
            ASSERT(left == nullptr && right == nullptr);
            buildLeaf(node);

        } else {
            ASSERT(left != nullptr && right != nullptr);
            buildInner(node, *left, *right);
        }
        return true;
    });
}

Vector BarnesHut::eval(const Size idx) {
    return this->evalImpl(r[idx], idx);
}

Vector BarnesHut::eval(const Vector& r0) {
    return this->evalImpl(r0, Size(-1));
}

Vector BarnesHut::evalImpl(const Vector& r0, const Size idx) {
    if (SPH_UNLIKELY(r.empty())) {
        return Vector(0._f);
    }
    Vector f(0._f);
    kdTree.iterate<KdTree::Direction::TOP_DOWN>(
        [this, &r0, &f, idx](KdNode& node, KdNode* UNUSED(left), KdNode* UNUSED(right)) {
            if (node.box == Box::EMPTY()) {
                // no particles in this node, skip
                return false;
            }
            const Float boxSizeSqr = getSqrLength(node.box.size());
            const Float boxDistSqr = getSqrLength(node.box.center() - r0);
            ASSERT(isReal(boxDistSqr));
            if (boxSizeSqr / (boxDistSqr + EPS) < thetaSqr) {
                // small node, use multipole approximation
                f += evaluateGravity(r0 - node.com, node.moments, int(order));

                // skip the children
                return false;
            } else {
                // too large box; if inner, recurse into children, otherwise sum each particle of the leaf
                if (node.isLeaf()) {
                    LeafNode& leaf = (LeafNode&)node;
                    LeafIndexSequence sequence = kdTree.getLeafIndices(leaf);
                    for (Size i : sequence) {
                        if (idx == i) {
                            continue;
                        }
                        const Vector dr = r[i] - r0;
                        f += m[i] * dr / pow<3>(getLength(dr));
                    }
                    return false; // return value doesn't matter here
                } else {
                    // continue with children
                    return true;
                }
            }
        });
    return Constants::gravity * f;
}

void BarnesHut::buildLeaf(KdNode& node) {
    LeafNode& leaf = (LeafNode&)node;
    if (leaf.size() == 0) {
        // set to zero to correctly compute mass and com of parent nodes
        leaf.com = Vector(0._f);
        leaf.moments.order<0>() = 0._f;
        leaf.moments.order<1>() = TracelessMultipole<1>(0._f);
        leaf.moments.order<2>() = TracelessMultipole<2>(0._f);
        leaf.moments.order<3>() = TracelessMultipole<3>(0._f);
        return;
    }
    // compute the center of gravity (the box is already done)
    leaf.com = Vector(0._f);
    Float m_leaf = 0._f;
    LeafIndexSequence sequence = kdTree.getLeafIndices(leaf);
    for (Size i : sequence) {
        leaf.com += m[i] * r[i];
        m_leaf += m[i];
    }
    ASSERT(m_leaf > 0._f, m_leaf);
    leaf.com /= m_leaf;
    ASSERT(isReal(leaf.com) && getLength(leaf.com) < LARGE, leaf.com);

    // compute gravitational moments from individual particles
    // M0 is a sum of particle masses, M1 is a dipole moment = zero around center of mass
    ASSERT(computeMultipole<0>(r, m, leaf.com, sequence).value() == m_leaf);
    const Multipole<2> m2 = computeMultipole<2>(r, m, leaf.com, sequence);
    const Multipole<3> m3 = computeMultipole<3>(r, m, leaf.com, sequence);

    // compute traceless tensors to reduce number of independent components
    const TracelessMultipole<2> q2 = computeReducedMultipole(m2);
    const TracelessMultipole<3> q3 = computeReducedMultipole(m3);

    // save the moments to the leaf
    leaf.moments.order<0>() = m_leaf;
    leaf.moments.order<1>() = TracelessMultipole<1>(0._f);
    leaf.moments.order<2>() = q2;
    leaf.moments.order<3>() = q3;

    // sanity check
    ASSERT(leaf.size() > 1 || q2 == TracelessMultipole<2>(0._f));
    ASSERT(leaf.size() > 1 || q3 == TracelessMultipole<3>(0._f));
}

void BarnesHut::buildInner(KdNode& node, KdNode& left, KdNode& right) {
    InnerNode& inner = (InnerNode&)node;

    // update bounding box
    inner.box = Box::EMPTY();
    inner.box.extend(left.box);
    inner.box.extend(right.box);

    // update center of mass
    const Float ml = left.moments.order<0>();
    const Float mr = right.moments.order<0>();

    // check for empty node
    if (ml + mr == 0._f) {
        // set to zero to correctly compute sum and com of parent nodes
        inner.com = Vector(0._f);
        inner.moments.order<0>() = 0._f;
        inner.moments.order<1>() = TracelessMultipole<1>(0._f);
        inner.moments.order<2>() = TracelessMultipole<2>(0._f);
        inner.moments.order<3>() = TracelessMultipole<3>(0._f);
        return;
    }

    inner.com = (ml * left.com + mr * right.com) / (ml + mr);
    ASSERT(isReal(inner.com) && getLength(inner.com) < LARGE, inner.com);

    inner.moments.order<0>() = ml + mr;

    // we already computed moments of children nodes, sum up using parallel axis theorem
    Vector d = left.com - inner.com;
    TracelessMultipole<1>& Ml1 = left.moments.order<1>();
    TracelessMultipole<2>& Ml2 = left.moments.order<2>();
    TracelessMultipole<3>& Ml3 = left.moments.order<3>();
    inner.moments.order<1>() = parallelAxisTheorem(Ml1, ml, d);
    inner.moments.order<2>() = parallelAxisTheorem(Ml2, ml, d);
    inner.moments.order<3>() = parallelAxisTheorem(Ml3, Ml2, ml, d);

    d = right.com - inner.com;
    TracelessMultipole<1>& Mr1 = right.moments.order<1>();
    TracelessMultipole<2>& Mr2 = right.moments.order<2>();
    TracelessMultipole<3>& Mr3 = right.moments.order<3>();
    inner.moments.order<1>() += parallelAxisTheorem(Mr1, mr, d);
    inner.moments.order<2>() += parallelAxisTheorem(Mr2, mr, d);
    inner.moments.order<3>() += parallelAxisTheorem(Mr3, Mr2, mr, d);
}

MultipoleExpansion<3> BarnesHut::getMoments() const {
    return kdTree.getNode(0).moments;
}

NAMESPACE_SPH_END
