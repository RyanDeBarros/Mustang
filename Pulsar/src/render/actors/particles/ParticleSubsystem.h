#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "Utils.h"
#include "Particle.h"

typedef unsigned char ParticleSubsystemIndex;

template<std::unsigned_integral ParticleCount = unsigned short>
struct ParticleSubsystemData
{
	real wavePeriod;
	std::shared_ptr<DebugPolygon> prototypeShape;
	CumulativeFunc<ParticleCount> spawnFunc;
	std::function<real(const Particles::CHRSeed&)> lifespanFunc;
	Particles::CharacteristicGen characteristicGen;
};

template<std::unsigned_integral ParticleCount>
class ParticleEffect;

template<std::unsigned_integral ParticleCount = unsigned short>
class ParticleSubsystem : public TransformableProxy2D, public std::enable_shared_from_this<ParticleSubsystem<ParticleCount>>
{
	CumulativeFunc<ParticleCount> m_SpawnFunc;
	std::function<real(const Particles::CHRSeed&)> m_LifespanFunc;
	Particles::CharacteristicGen m_CharacteristicGen;
	real m_Period;
	real m_PeriodInv;
	std::shared_ptr<DebugPolygon> m_Shape;
	ParticleCount m_NumSpawned = 0;
	unsigned int m_TotalSpawn;
	unsigned int m_WaveNum = 0;
	ParticleSubsystemIndex m_SubsystemIndex;

public:
	ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index);
	ParticleSubsystem(const ParticleSubsystem<ParticleCount>&) = delete;
	ParticleSubsystem(ParticleSubsystem<ParticleCount>&&) noexcept;
	ParticleSubsystem<ParticleCount>& operator=(const ParticleSubsystem<ParticleCount>&) = delete;
	ParticleSubsystem<ParticleCount>& operator=(ParticleSubsystem<ParticleCount>&&) noexcept;

	inline void SetWavePeriod(real wave_period) { m_Period = wave_period; m_PeriodInv = 1.0f / m_Period; }

private:
	friend class ParticleEffect<ParticleCount>;
	void OnUpdate(ParticleEffect<ParticleCount>& psys);
	void OnSpawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed);
};
