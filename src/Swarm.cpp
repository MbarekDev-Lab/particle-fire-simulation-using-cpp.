// ============================================================================
// Swarm.cpp — Particle collection management
//
// Author      : M'Barek Benraiss
// Description : Creates and bulk-updates particles with delta-time
//
// Changes from original course code:
//   • Constructor takes configurable count (was hardcoded constant).
//   • Uses range-based for loop — cleaner than index-based iteration.
//   • Each Particle's constructor calls initRandom() automatically,
//     so the swarm is ready to render immediately after construction.
// ============================================================================

#include "Swarm.h"

namespace particle_sim {

Swarm::Swarm(int count) : m_lastUpdateTime(0) {
    // Pre-allocate all particles; each one randomizes itself in its constructor.
    m_particles.resize(static_cast<std::size_t>(count));
}

void Swarm::update(int elapsedMs) {
    // Delta time = time since last update.
    // First frame: interval equals elapsedMs (since m_lastUpdateTime starts at 0).
    int interval = elapsedMs - m_lastUpdateTime;

    for (auto& particle : m_particles) {
        particle.update(interval);
    }

    m_lastUpdateTime = elapsedMs;
}

} // namespace particle_sim
