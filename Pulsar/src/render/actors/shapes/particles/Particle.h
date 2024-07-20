#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>

#include "Utils.h"
#include "render/actors/shapes/DebugPolygon.h"
#include "render/Transform.h"

// TODO throughout project, pass by value for built-in types instead of const&.
struct Particle;
namespace Particles {

	struct CHRSeedData
	{
		float* s = nullptr;
		unsigned char n;
		CHRSeedData(unsigned char n)
			: n(n)
		{
			s = new float[n];
			for (unsigned char i = 0; i < n; i++)
				s[i] = rng();
		}
		CHRSeedData(const CHRSeedData& data)
			: n(data.n)
		{
			memcpy_s(s, n, data.s, n);
		}
		CHRSeedData(CHRSeedData&& data) noexcept
			: n(data.n), s(data.s)
		{
			data.s = nullptr;
		}
		~CHRSeedData()
		{
			if (s)
				delete[] s;
		}
	};

	struct Characteristic : public std::function<void(Particle&)>
	{
		Characteristic(const std::function<void(Particle&, const std::shared_ptr<CHRSeedData>&)>& f, unsigned char num_seeds)
			: std::function<void(Particle&)>(std::bind(f, std::placeholders::_1, std::shared_ptr<CHRSeedData>(new CHRSeedData(num_seeds)))) {}
		Characteristic(const std::function<void(Particle&)>& f) : std::function<void(Particle&)>(f) {}
	};

	typedef std::function<std::vector<Characteristic>(real, unsigned short, unsigned int)> CharacteristicVecGen;
	typedef std::function<real(real)> TimeFunc;
}

struct Particle
{
	LocalTransformer2D m_Transformer;
	std::shared_ptr<DebugPolygon> m_Shape;
	std::vector<Particles::Characteristic> m_Characteristics;

private:
	template<std::unsigned_integral ParticleCount>
	friend class ParticleSystem;
	real m_LifespanInv;
	real m_T = 0.0f;
	real m_DT = 0.0f;
	bool m_Invalid = false;

public:
	Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan,
		const std::vector<Particles::Characteristic>& characteristics);
	Particle& operator=(const Particle&);
	
	inline real t() const { return m_T; }
	inline real dt() const { return m_DT; }

private:
	void OnDraw();

};
