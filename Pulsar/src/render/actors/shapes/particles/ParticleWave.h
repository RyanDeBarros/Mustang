#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "Utils.h"
#include "Particle.h"

template<std::unsigned_integral ParticleCount = unsigned short>
struct ParticleWaveData
{
	real wavePeriod;
	std::shared_ptr<DebugPolygon> prototypeShape;
	CumulativeFunc<ParticleCount> spawnFunc;
	std::function<real(const Particles::CHRSeed&)> lifespanFunc;
	Particles::CharacteristicGen characteristicGen;
};

template<std::unsigned_integral ParticleCount>
class ParticleSystem;

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleWave
{
	CumulativeFunc<ParticleCount> m_SpawnFunc;
	std::function<real(const Particles::CHRSeed&)> m_LifespanFunc;
	Particles::CharacteristicGen m_CharacteristicGen;
	real m_WavePeriod;
	real m_WavePeriodInv;
	std::shared_ptr<DebugPolygon> m_Shape;
	ParticleCount m_NumSpawned = 0;
	unsigned int m_TotalSpawn;
	unsigned int m_WaveNum = 0;

public:
	ParticleWave(const ParticleWaveData<ParticleCount>& wave_data);

	inline void SetWavePeriod(real wave_period) { m_WavePeriod = wave_period; m_WavePeriodInv = 1.0f / m_WavePeriod; }

private:
	friend class ParticleSystem<ParticleCount>;
	void OnUpdate(float delta_time, ParticleSystem<ParticleCount>& psys);
	void OnSpawn(ParticleSystem<ParticleCount>& psys, const Particles::CHRSeed& seed);
};
