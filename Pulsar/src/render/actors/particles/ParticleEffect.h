#pragma once

#include <vector>

#include "Particle.h"
#include "ParticleSubsystem.h"
#include "../../transform/MultiTransformer.h"

// TODO add global modulation. perhaps use a special Transformer that propogates modulation. A Modulator?
template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleEffect
{
	friend class ParticleSubsystem<ParticleCount>;
	std::vector<std::vector<Particle>> m_Particles;
	real m_TotalPlayed = 0.0f;
	real m_DeltaTime = 0.0f;
	real m_PlayTime = 0.0f;
	real m_LeftoverDT = 0.0f;

public:
	ParticleEffect(const std::vector<ParticleSubsystemData<ParticleCount>>& subsystem_data, const Transform2D& transform = {}, bool enabled = true);
	ParticleEffect(const ParticleEffect<ParticleCount>&) = delete;
	ParticleEffect(ParticleEffect<ParticleCount>&&) = delete;

	/// Whether the particle system is updated every update frame. This does not affect whether the system should be drawn.
	bool enabled;
	/// Whether the particles are spawning. This does not affect currently spawned particles.
	bool spawning = true;
	/// Whether the internal timer is updating every update frame.
	bool paused = false;

	void Pause();
	void Resume();
	inline void Reset() { m_TotalPlayed = 0.0f; m_DeltaTime = 0.0f; m_PlayTime = 0.0f; }
	void PlayFor(real n);

	inline MultiTransformer2D* Transformer() { return &m_Transformer; }
	inline ParticleSubsystem<ParticleCount>& SubsystemRef(unsigned int i) { return *m_Subsystems[i]; }
	inline std::shared_ptr<TransformableProxy2D> TransformRef() { return m_Transform; }

protected:
	std::vector<std::shared_ptr<ParticleSubsystem<ParticleCount>>> m_Subsystems;
	std::shared_ptr<TransformableProxy2D> m_Transform;
	MultiTransformer2D m_Transformer;

	void OnUpdate();
	virtual void DespawnInvalidParticles();
	virtual void InvalidateParticle(const Particle& part, ParticleSubsystemIndex i) {};
	virtual void AddParticle(ParticleSubsystemIndex i, const Particle& part);
	virtual void AddParticle(ParticleSubsystemIndex i, Particle&& part);

private:
	void OnSubsystemsUpdate();
	void OnParticlesUpdate();
};
