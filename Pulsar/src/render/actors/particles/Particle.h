#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "../shapes/DebugPolygon.h"
#include "../../transform/Transformer.h"

struct Particle;
namespace Particles {

	typedef unsigned char DataIndex;

	typedef std::function<void(Particle& p)> CHRFunc;
	typedef std::pair<CHRFunc, DataIndex> CHRBind;
	
	struct CHRSeed
	{
		real waveT;
		unsigned int spawnIndex;
		unsigned int totalSpawn;
	};

	typedef std::function<CHRBind(const CHRSeed&)> CharacteristicGen;
}

struct Particle
{
	Transformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	Particles::CHRFunc m_Characteristic;
	float* m_Data = nullptr;
	Particles::DataIndex m_DataSize;

private:
	template<std::unsigned_integral ParticleCount>
	friend class ParticleEffect;
	real m_LifespanInv;
	real m_T = 0.0f;
	real m_DT = 0.0f;
	bool m_Invalid = false;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const Transformer2D& transformer, const float& lifespan, const Particles::CHRBind& characteristic);
	Particle(const Particle&);
	Particle(Particle&&) noexcept;
	Particle& operator=(const Particle&);
	Particle& operator=(Particle&&) noexcept;
	~Particle();

	inline float& operator[](Particles::DataIndex i) { return m_Data[i]; }
	
	inline real t() const { return m_T; }
	inline real dt() const { return m_DT; }

private:
	void OnDraw(real delta_time);

};
