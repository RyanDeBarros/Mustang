#pragma once

#include <vector>
#include <variant>

#include "Pulsar.h"
#include "Typedefs.h"
#include "render/ActorRenderBase.h"
#include "render/Transform.h"
#include "render/actors/shapes/DebugBatcher.h"
#include "Particle.h"
#include "ParticleWave.h"

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSystem : public ActorRenderBase2D, public Transformable2D
{
	friend class ParticleWave<ParticleCount>;
	DebugBatcher m_Batcher;
	std::vector<Particle> m_Particles;
	std::vector<ParticleWave<ParticleCount>> m_Waves;
	real m_TotalPlayed = 0.0f;
	real m_DeltaTime = 0.0f;
	real m_PlayTime = 0.0f;
	real m_LeftoverDT = 0.0f;
	
public:
	ParticleSystem(const std::vector<ParticleWaveData<ParticleCount>>& wave_data, const Transform2D& transform = {}, ZIndex z = 0, bool visible = true, bool enabled = true);

	/// Whether the particle system should be drawn. This does not affect whether the system is updated every update frame.
	bool visible;
	/// Whether the particle system is updated every update frame. This does not affect whether the system should be drawn.
	bool enabled;
	/// Whether the particles are spawning. This does not affect currently spawned particles.
	bool spawning = true;
	/// Whether the internal timer is updating every update frame.
	bool paused = false;
	
	virtual void RequestDraw(class CanvasLayer* canvas_layer) override;
	void OnWavesUpdate();
	void OnParticlesUpdate();

	void Pause();
	void Resume();
	inline void Reset() { /*m_LifetimeStart = Pulsar::totalDrawTime;*/ m_TotalPlayed = 0.0f; m_DeltaTime = 0.0f; m_PlayTime = 0.0f; }
	void PlayFor(real n);
};
