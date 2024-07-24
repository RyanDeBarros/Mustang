#include "Particle.h"

#include "Pulsar.h"
#include "Utils.h"

Particle::Particle(const std::shared_ptr<DebugPolygon>& shape, const LocalTransformer2D& transformer, const float& lifespan, const Particles::CHRBind& characteristic)
	: m_Shape(shape), m_Transformer(transformer), m_Characteristic(characteristic.first), m_LifespanInv(1.0f / lifespan), m_DataSize(characteristic.second)
{
	if (m_DataSize > 0)
		m_Data = new float[m_DataSize];
	m_Characteristic(*this);
}

Particle::Particle(Particle&& other) noexcept
	: m_Shape(other.m_Shape), m_Transformer(other.m_Transformer), m_Characteristic(other.m_Characteristic), m_LifespanInv(other.m_LifespanInv), m_Data(other.m_Data), m_DataSize(other.m_DataSize), m_Invalid(other.m_Invalid), m_T(other.m_T), m_DT(other.m_DT)
{
	other.m_Data = nullptr;
	m_Characteristic(*this);
}

Particle::~Particle()
{
	if (m_Data)
		delete[] m_Data;
}

Particle& Particle::operator=(const Particle& other)
{
	m_Shape = other.m_Shape;
	m_Transformer = other.m_Transformer;
	m_Characteristic = other.m_Characteristic;
	m_LifespanInv = other.m_LifespanInv;
	m_Invalid = other.m_Invalid;
	m_T = other.m_T;
	m_DT = other.m_DT;
	m_DataSize = other.m_DataSize;
	if (m_DataSize > 0)
	{
		m_Data = new float[m_DataSize];
		memcpy_s(m_Data, m_DataSize, other.m_Data, m_DataSize);
	}
	else
	{
		m_Data = nullptr;
	}
	return *this;
}

void Particle::OnDraw()
{
	m_DT = Pulsar::deltaDrawTime * m_LifespanInv;
	if ((m_T += m_DT) > 1.0f)
		m_Invalid = true;
	else
		m_Characteristic(*this);
}

namespace Particles {

	CharacteristicGen CombineSequential(const std::vector<CharacteristicGen>& characteristics)
	{
		return [characteristics](const CHRSeed& seed) {
			std::vector<CHRFunc> funcs;
			unsigned char num_seeds = 0;
			for (const auto& gen : characteristics)
			{
				const auto& g = gen(seed);
				funcs.push_back(g.first);
				if (g.second > num_seeds)
					num_seeds = g.second;
			}
			return CHRBind{ [funcs](Particle& p) {
				for (const auto& f : funcs)
					f(p);
			}, num_seeds };
		};
	}
	
	CharacteristicGen CombineInitialOverTime(const CharacteristicGen& initial, const CharacteristicGen& over_time)
	{
		return [initial, over_time](const CHRSeed& seed)
		{
			const auto& f_initial = initial(seed);
			const auto& f_over_time = over_time(seed);
			return CHRBind{
				[f_initial, f_over_time](Particle& p) {
				if (p.t() == 0.0f)
					f_initial.first(p);
				else
					f_over_time.first(p);
			}, f_initial.second > f_over_time.second ? f_initial.second : f_over_time.second };
		};
	}

}
