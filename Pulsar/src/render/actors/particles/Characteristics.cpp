#include "Characteristics.h"

#include "utils/CommonMath.h"
#include "utils/Strings.h"

namespace Particles {

	CharacteristicGen CombineSequential(const std::vector<CharacteristicGen>& characteristics)
	{
		return [characteristics](const CHRSeed& seed) {
			std::vector<CHRFunc> funcs;
			DataIndex max_index = 0;
			for (const auto& gen : characteristics)
			{
				const auto& g = gen(seed);
				funcs.push_back(g.first);
				if (g.second > max_index)
					max_index = g.second;
			}
			return CHRBind{ [funcs](Particle& p) {
				for (const auto& f : funcs)
					f(p);
			}, max_index };
		};
	}

	CharacteristicGen GenSetup(const CharacteristicGen& setup)
	{
		return [setup](const CHRSeed& seed) {
			const auto& f_setup = setup(seed);
			return CHRBind{
				[f_setup](Particle& p) {
				if (p.t() == 0.0f)
					f_setup.first(p);
			}, f_setup.second };
		};
	}

	CharacteristicGen CombineInitialOverTime(const CharacteristicGen& initial, const CharacteristicGen& over_time)
	{
		return [initial, over_time](const CHRSeed& seed) {
			const auto& f_initial = initial(seed);
			const auto& f_over_time = over_time(seed);
			return CHRBind{
				[f_initial, f_over_time](Particle& p) {
				if (p.t() == 0.0f)
					f_initial.first(p);
				else
					f_over_time.first(p);
			}, std::max(f_initial.second, f_over_time.second) };
		};
	}

	CharacteristicGen CombineIntervals(const CharacteristicGen& first, const CharacteristicGen& second, float divider, bool or_equal)
	{
		return [first, second, divider, or_equal](const CHRSeed& seed) {
			const auto& f_first = first(seed);
			const auto& f_second = second(seed);
			return CHRBind{
				[f_first, f_second, divider, or_equal](Particle& p) {
				if ((p.t() < divider) || (or_equal && p.t() <= divider))
					f_first.first(p);
				else
					f_second.first(p);
			}, std::max(f_first.second, f_second.second) };
		};
	}

	CharacteristicGen CombineConditionalDataLessThan(DataIndex di, float compared_to, const CharacteristicGen& first, const CharacteristicGen& second)
	{
		return [di, compared_to, first, second](const CHRSeed& seed) {
			const auto& f_first = first(seed);
			const auto& f_second = second(seed);
			return CHRBind{
				[di, compared_to, f_first, f_second](Particle& p) {
				if (p[di] < compared_to)
					f_first.first(p);
				else
					f_second.first(p);
			}, Max(di + 1, f_first.second, f_second.second)};
		};
	}

	CharacteristicGen CombineConditionalTimeLessThan(float compared_to, const CharacteristicGen& first, const CharacteristicGen& second)
	{
		return [compared_to, first, second](const CHRSeed& seed) {
			const auto& f_first = first(seed);
			const auto& f_second = second(seed);
			return CHRBind{
				[compared_to, f_first, f_second](Particle& p) {
				if (p.t() < compared_to)
					f_first.first(p);
				else
					f_second.first(p);
			}, std::max(f_first.second, f_second.second) };
		};
	}

	namespace CHR {

		CharacteristicGen FeedDataRNG(DataIndex si)
		{
			return [si](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si](Particle& p)
					{
						p[si] = rng();
					}, si + 1
				};
			};
		}

		CharacteristicGen FeedData(DataIndex si, float v)
		{
			return [si, v](const Particles::CHRSeed& seed)
				{
					return Particles::CHRBind{
						[si, v](Particle& p)
						{
							p[si] = v;
						}, si + 1
					};
				};
		}

		CharacteristicGen FeedDataRandSpawnIndex(DataIndex si)
		{
			return [si](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si, &seed](Particle& p)
					{
						p[si] = static_cast<float>(std::rand() % seed.totalSpawn);
					}, si + 1
				};
			};
		}

		CharacteristicGen FeedDataRandBinChoice(DataIndex si, float option1, float option2)
		{
			return [si, option1, option2](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si, option1, option2](Particle& p)
					{
						p[si] = std::rand() % 2 == 0 ? option1 : option2;
					}, si + 1
				};
			};
		}

		CharacteristicGen FeedDataShiftMod(DataIndex si, DataIndex shiftI, float mod)
		{
			return [si, shiftI, mod](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si, shiftI, mod](Particle& p)
					{
						p[si] = std::fmod(p.t() + shiftI, mod);
					}, std::max(si, shiftI) + 1
				};
			};
		}

		CharacteristicGen FeedDataTime(DataIndex si)
		{
			return [si](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si](Particle& p)
					{
						p[si] = p.t();
					}, si + 1
				};
			};
		}

		CharacteristicGen OperateData(DataIndex si, const std::function<void(float&)>& func)
		{
			return [si, func](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si, func](Particle& p)
					{
						func(p[si]);
					}, si + 1
				};
			};
		}

		CharacteristicGen OperateData(DataIndex si, const std::function<std::function<void(float&)>(float)>& func, DataIndex di)
		{
			return [si, di, func](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si, di, func](Particle& p)
					{
						func(p[di])(p[si]);
					}, std::max(si, di) + 1
				};
			};
		}

		CharacteristicGen OperateData(DataIndex si1, const std::function<void(float&)>& func1, DataIndex si2, const std::function<void(float&)>& func2)
		{
			return [si1, func1, si2, func2](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si1, func1, si2, func2](Particle& p)
					{
						func1(p[si1]);
						func2(p[si2]);
					}, std::max(si1, si2) + 1
				};
			};
		}

		CharacteristicGen OperateData(DataIndex si1, const std::function<std::function<void(float&)>(float)>& func1, DataIndex di1, DataIndex si2, const std::function<std::function<void(float&)>(float)>& func2, DataIndex di2)
		{
			return [si1, di1, func1, si2, di2, func2](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[si1, di1, func1, si2, di2, func2](Particle& p)
					{
						func1(p[di1])(p[si1]);
						func2(p[di2])(p[si2]);
					}, Max(si1, si2, di1, di2) + 1
				};
			};
		}

		CharacteristicGen SetColorUsingData(const std::function<glm::vec4(float)>& color, DataIndex di)
		{
			return [color, di](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[color, di](Particle& p)
					{
						p.m_Modulator->SetLocalColor(p.ti(), color(p[di]));
					}, di + 1
				};
			};
		}

		CharacteristicGen SetColorUsingTime(const std::function<glm::vec4(float)>& color)
		{
			return [color](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[color](Particle& p)
					{
						p.m_Modulator->SetLocalColor(p.ti(), color(p.t()));
					}, 0
				};
			};
		}

		CharacteristicGen SetLocalScaleUsingData(const std::function<glm::vec2(float)>& scale, DataIndex di)
		{
			return [scale, di](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[scale, di](Particle& p)
					{
						p.m_Transformer->SetLocalScale(p.ti(), scale(p[di]));
					}, di + 1
				};
			};
		}

		CharacteristicGen SetLocalPositionUsingData(const std::function<glm::vec2(float)>& position, DataIndex di)
		{
			return [position, di](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[position, di](Particle& p)
					{
						p.m_Transformer->SetLocalPosition(p.ti(), position(p[di]));
					}, di + 1
				};
			};
		}

		CharacteristicGen SetLocalPositionUsingData(const std::function<glm::vec2(glm::vec2)>& position, DataIndex di1, DataIndex di2)
		{
			return [position, di1, di2](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[position, di1, di2](Particle& p)
					{
						p.m_Transformer->SetLocalPosition(p.ti(), position({p[di1], p[di2]}));
					}, Max(di1, di2) + 1
				};
			};
		}

		CharacteristicGen OperateLocalPositionFromVelocityData(DataIndex dix, DataIndex diy)
		{
			return [dix, diy](const Particles::CHRSeed& seed)
			{
				return Particles::CHRBind{
					[dix, diy](Particle& p)
					{
						p.m_Transformer->OperateLocalPosition(p.ti(), [&p, dix, diy](glm::vec2& pos) { pos += glm::vec2{p[dix], p[diy]} * p.dt(); });
					}, Max(dix, diy) + 1
				};
			};
		}

	}

}

