#pragma once

#include <vector>
#include <functional>

#include "Utils.h"
#include "Particle.h"

namespace Particles {
	typedef std::function<float(float t, float seed)> FloatFunc;
	typedef std::function<std::vector<ParticleProfileFunc>(float t, float seed)> ProfileFuncVectorFunc;
}

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleWave
{
	CumulativeFunc<ParticleCount> m_SpawnFunc;
	ParticleCount num_spawned;
	Particles::FloatFunc m_LifespanFunc;
	Particles::ProfileFuncVectorFunc m_ProfileFuncs;
	Particles::ProfileFuncVectorFunc m_InitFuncs;
	float m_WavePeriod;
	float m_WavePeriodInv;
	DebugPolygon m_Shape;

public:
	ParticleWave(float wave_period, const DebugPolygon& prototype_shape, const CumulativeFunc<ParticleCount>& spawn_func, const Particles::FloatFunc& lifespan_func, const Particles::ProfileFuncVectorFunc& profile_funcs, const Particles::ProfileFuncVectorFunc& init_funcs);

	void OnUpdate(float deltaTime, Transform2D const* const parent_transform, std::vector<Particle>& particles);
	
	inline void SetWavePeriod(float wave_period) { m_WavePeriod = wave_period; m_WavePeriodInv = 1.0f / m_WavePeriod; }

private:
	void OnSpawn(float t, Transform2D const* const parent_transform, std::vector<Particle>& particles);
};
