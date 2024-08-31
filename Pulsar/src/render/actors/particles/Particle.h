#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "Pulsar.h"
#include "../shapes/DebugPolygon.h"
#include "utils/Functor.inl"

struct Particle;
namespace Particles {

	typedef unsigned char DataIndex;

	struct CHRSeed
	{
		real waveT;
		unsigned int spawnIndex;
		unsigned int totalSpawn;
	};

	using CHRFunc = Functor<void, Particle&>;

	using CHRBind = std::pair<CHRFunc, DataIndex>;

	using CharacteristicGen = Functor<CHRBind, const CHRSeed&>;
}

struct Particle
{
	std::shared_ptr<DebugPolygon> m_Shape;
	Particles::CHRFunc m_Characteristic;
	float* m_Data = nullptr;
	Particles::DataIndex m_DataSize;

private:
	bool m_Invalid = false;
	friend class ParticleEffect;
	friend class ParticleSubsystem;
	real m_LifespanInv;
	real m_T = 0.0f;
	real m_DT = 0.0f;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const float& lifespan, Particles::CHRBind&& characteristic);
	Particle(const Particle&) = delete;
	Particle(Particle&&) noexcept;
	Particle& operator=(const Particle&) = delete;
	Particle& operator=(Particle&&) noexcept;
	~Particle();

	float& operator[](Particles::DataIndex i) { return m_Data[i]; }
	
	real t() const { return m_T; }
	real dt() const { return m_DT; }

private:
	void OnDraw(real delta_time);

};
