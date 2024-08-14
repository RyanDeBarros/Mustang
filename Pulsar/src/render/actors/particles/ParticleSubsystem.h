#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Typedefs.h"
#include "utils/CommonMath.h"
#include "Particle.h"
#include "../../transform/Transform.h"
#include "../../transform/Modulate.h"

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
class ParticleSubsystem
{
	ParticleSubsystemData<ParticleCount> m_Data;
	real m_PeriodInv;
	ParticleCount m_NumSpawned = 0;
	unsigned int m_TotalSpawn;
	unsigned int m_WaveNum = 0;
	ParticleSubsystemIndex m_SubsystemIndex;
	Transformer2D m_Transformer;
	Modulator m_Modulator;
	std::vector<Particle> m_Particles;

public:
	ParticleSubsystem(const ParticleSubsystemData<ParticleCount>& wave_data, ParticleSubsystemIndex subsystem_index);
	ParticleSubsystem(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem(ParticleSubsystem<ParticleCount>&&) noexcept;
	ParticleSubsystem<ParticleCount>& operator=(const ParticleSubsystem<ParticleCount>&);
	ParticleSubsystem<ParticleCount>& operator=(ParticleSubsystem<ParticleCount>&&) noexcept;

	inline void SetWavePeriod(real wave_period) { m_Data.wavePeriod = wave_period; m_PeriodInv = 1.0f / m_Data.wavePeriod; }

	Transformer2D* Transformer() { return &m_Transformer; }

private:
	friend class ParticleEffect<ParticleCount>;
	void OnSpawnFrame(ParticleEffect<ParticleCount>& psys);
	void Spawn(ParticleEffect<ParticleCount>& psys, const Particles::CHRSeed& seed);
	void OnParticlesUpdate(ParticleEffect<ParticleCount>& psys);
	void RemoveUnordered(ParticleCount i);
};
