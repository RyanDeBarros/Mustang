#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "Utils.h"
#include "Particle.h"

namespace Particles {
	typedef std::function<float(real t, float seed)> FloatFunc;
	typedef std::function<std::vector<ParticleProfileFunc>(real t, float seed)> ProfileFuncVectorFunc;
}

template<std::unsigned_integral ParticleCount = unsigned short>
struct ParticleWaveData
{
	real wavePeriod;
	std::shared_ptr<DebugPolygon> prototypeShape;
	CumulativeFunc<ParticleCount> spawnFunc;
	Particles::FloatFunc lifespanFunc;
	Particles::ProfileFuncVectorFunc profileFuncs;
	Particles::ProfileFuncVectorFunc initFuncs;
};

template<std::unsigned_integral ParticleCount>
class ParticleSystem;

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleWave
{
	CumulativeFunc<ParticleCount> m_SpawnFunc;
	ParticleCount num_spawned;
	Particles::FloatFunc m_LifespanFunc;
	Particles::ProfileFuncVectorFunc m_ProfileFuncs;
	Particles::ProfileFuncVectorFunc m_InitFuncs;
	real m_WavePeriod;
	real m_WavePeriodInv;
	std::shared_ptr<DebugPolygon> m_Shape;

public:
	ParticleWave(const ParticleWaveData<ParticleCount>& wave_data);

	inline void SetWavePeriod(real wave_period) { m_WavePeriod = wave_period; m_WavePeriodInv = 1.0f / m_WavePeriod; }

private:
	friend class ParticleSystem<ParticleCount>;
	void OnUpdate(float delta_time, ParticleSystem<ParticleCount>& psys);
	void OnSpawn(float t, ParticleSystem<ParticleCount>& psys);
};
