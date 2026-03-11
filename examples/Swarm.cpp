// ============================================================================
// Swarm.cpp — Practice implementation: bulk particle update with delta time
//
// Author : M'Barek Benraiss
//
// Enhancements over original:
//   • std::vector replaces raw new/delete — no memory leaks possible
//   • Delta-time update: computes interval from elapsed to decouple
//     simulation speed from frame rate
// ============================================================================

#include "Swarm.h"

namespace particle_example {

Swarm::Swarm() : m_particles(NPARTICLES), m_lastUpdateTime(0) {}

void Swarm::update(int elapsedMs) {
    int interval = elapsedMs - m_lastUpdateTime;
    m_lastUpdateTime = elapsedMs;

    for (auto& p : m_particles) {
        p.update(interval);
    }
}

} // namespace particle_example
