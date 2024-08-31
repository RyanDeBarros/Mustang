#include "Characteristics.h"

#include "utils/CommonMath.h"
#include "utils/Strings.h"
#include "utils/Data.inl"

// NOTE all fickle pointers are assumed to be non-null.

namespace Particles {

	CharacteristicGen Characterize(CHRFunc&& f, DataIndex max_index)
	{
		using cls = std::tuple<CHRFunc, DataIndex>;
		return make_functor_ptr<false>([](const CHRSeed&, const cls& tuple) -> CHRBind {
			return { make_functor_ptr<false>([](Particle& p, const CHRFunc& f) -> void {
				f(p);
			}, std::get<0>(tuple)), std::get<1>(tuple) };
		}, cls(std::move(f), max_index));
	}

	CharacteristicGen Characterize(FunctorPtr<void, const std::tuple<Particle&, const CHRSeed&>&>&& f, DataIndex max_index)
	{
		using tpl = std::tuple<Particle&, const CHRSeed&>;
		using cls = std::tuple<FunctorPtr<void, const tpl&>, DataIndex>;
		return make_functor_ptr<false>([](const CHRSeed& seed, const cls& tuple) -> CHRBind {
			using cls_inner = std::tuple<FunctorPtr<void, const tpl&>, const CHRSeed&>;
			return { make_functor_ptr<false>([](Particle& p, const cls_inner& tuple) -> void {
				std::get<0>(tuple)(tpl(p, std::get<1>(tuple)));
			}, cls_inner(std::get<0>(tuple), seed)), std::get<1>(tuple) };
		}, cls(std::move(f), max_index));
	}

	CharacteristicGen CombineSequential(std::vector<CharacteristicGen>&& characteristics)
	{
		return make_functor_ptr<false>([](const CHRSeed& seed, const std::vector<CharacteristicGen>& characteristics) -> CHRBind {
			std::vector<CHRFunc> funcs;
			DataIndex max_index = 0;
			for (const CharacteristicGen& gen : characteristics)
			{
				CHRBind g(gen(seed));
				funcs.push_back(std::move(g.first));
				if (g.second > max_index)
					max_index = g.second;
			}
			return { make_functor_ptr<false>([](Particle& p, const std::vector<CHRFunc>& funcs) -> void {
				for (auto& f : funcs)
					f(p);
			}, std::move(funcs)), max_index };
		}, std::move(characteristics));
	}

	CharacteristicGen GenSetup(CharacteristicGen&& setup)
	{
		return make_functor_ptr<false>([](const CHRSeed& seed, const CharacteristicGen& setup) -> CHRBind {
			CHRBind f_setup(setup(seed));
			return { make_functor_ptr<false>([](Particle& p, const CHRFunc& f) -> void {
				if (p.t() == 0.0f)
					f(p);
			}, std::move(f_setup.first)), f_setup.second };
		}, std::move(setup));
	}

	CharacteristicGen CombineInitialOverTime(CharacteristicGen&& initial, CharacteristicGen&& over_time)
	{
		using cls = std::pair<CharacteristicGen, CharacteristicGen>;
		return make_functor_ptr<false>([](const CHRSeed& seed, const cls& pair) -> CHRBind {
			CHRBind initial(pair.first(seed));
			CHRBind over_time(pair.second(seed));
			using cls_inner = std::pair<CHRFunc, CHRFunc>;
			return { make_functor_ptr<false>([](Particle& p, const cls_inner& pair) -> void {
				if (p.t() == 0.0f)
					pair.first(p);
				else
					pair.second(p);
			}, cls_inner(std::move(initial.first), std::move(over_time.first))), std::max(initial.second, over_time.second)};
		}, cls(std::move(initial), std::move(over_time)));
	}

	CharacteristicGen CombineIntervals(CharacteristicGen&& first, CharacteristicGen&& second, float divider, bool or_equal)
	{
		using cls = std::tuple<CharacteristicGen, CharacteristicGen, float, bool>;
		return make_functor_ptr<false>([](const CHRSeed& seed, const cls& tuple) -> CHRBind {
			CHRBind first(std::get<0>(tuple)(seed));
			CHRBind second(std::get<1>(tuple)(seed));
			using cls_inner = std::tuple<CHRFunc, CHRFunc, float, bool>;
			return { make_functor_ptr<false>([](Particle& p, const cls_inner& tuple) -> void {
				if ((p.t() < std::get<2>(tuple)) || (std::get<3>(tuple) && p.t() <= std::get<2>(tuple)))
					std::get<0>(tuple)(p);
				else
					std::get<1>(tuple)(p);
			}, cls_inner(std::move(first.first), std::move(second.first), std::get<2>(tuple), std::get<3>(tuple))), std::max(first.second, second.second) };
		}, cls(std::move(first), std::move(second), divider, or_equal));
	}

	CharacteristicGen CombineConditionalDataLessThan(DataIndex di, float compared_to, CharacteristicGen&& first, CharacteristicGen&& second)
	{
		using cls = std::tuple<DataIndex, float, CharacteristicGen, CharacteristicGen>;
		return make_functor_ptr<false>([](const CHRSeed& seed, const cls& tuple) -> CHRBind {
			CHRBind first(std::get<2>(tuple)(seed));
			CHRBind second(std::get<3>(tuple)(seed));
			using cls_inner = std::tuple<DataIndex, float, CHRFunc, CHRFunc>;
			return { make_functor_ptr<false>([](Particle& p, const cls_inner& tuple) -> void {
				if (p[std::get<0>(tuple)] < std::get<1>(tuple))
					std::get<2>(tuple)(p);
				else
					std::get<3>(tuple)(p);
			}, cls_inner(std::get<0>(tuple), std::get<1>(tuple), std::move(first.first), std::move(second.first))), Max(std::get<0>(tuple) + 1, first.second, second.second) };
		}, cls(di, compared_to, std::move(first), std::move(second)));
	}

	CharacteristicGen CombineConditionalTimeLessThan(float compared_to, CharacteristicGen&& first, CharacteristicGen&& second)
	{
		using cls = std::tuple<float, CharacteristicGen, CharacteristicGen>;
		return make_functor_ptr<false>([](const CHRSeed& seed, const cls& tuple) -> CHRBind {
			CHRBind first(std::get<1>(tuple)(seed));
			CHRBind second(std::get<2>(tuple)(seed));
			using cls_inner = std::tuple<float, CHRFunc, CHRFunc>;
			return { make_functor_ptr<false>([](Particle& p, const cls_inner& tuple) -> void {
				if (p.t() < std::get<0>(tuple))
					std::get<1>(tuple)(p);
				else
					std::get<2>(tuple)(p);
			}, cls_inner(std::get<0>(tuple), std::move(first.first), std::move(second.first))), std::max(first.second, second.second) };
		}, cls(compared_to, std::move(first), std::move(second)));
	}

	namespace CHR {
		
		CharacteristicGen FeedDataRNG(DataIndex si)
		{
			return Characterize(make_functor_ptr<true>([](Particle& p, DataIndex si) -> void {
				p[si] = rng();
			}, si), si + 1);
		}

		CharacteristicGen FeedData(DataIndex si, float v)
		{
			using cls = std::tuple<DataIndex, float>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				p[std::get<0>(tuple)] = std::get<1>(tuple);
			}, cls(si, v)), si + 1);
		}

		CharacteristicGen FeedDataRandSpawnIndex(DataIndex si)
		{
			return make_functor_ptr<true>([](const CHRSeed& seed, DataIndex si) -> CHRBind {
				using cls_inner = std::tuple<DataIndex, CHRSeed>;
				return { make_functor_ptr<false>([](Particle& p, const cls_inner& tuple) -> void {
					p[std::get<0>(tuple)] = static_cast<float>(std::rand() % std::get<1>(tuple).totalSpawn);
				}, cls_inner(si, seed)), si + 1 };
			}, si);
		}

		CharacteristicGen FeedDataRandBinChoice(DataIndex si, float option1, float option2)
		{
			using cls = std::tuple<DataIndex, float, float>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				// p[si] = std::rand() % 2 == 0 ? option1 : option2;
				p[std::get<0>(tuple)] = std::rand() % 2 == 0 ? std::get<1>(tuple) : std::get<2>(tuple);
			}, cls(si, option1, option2)), si + 1);
		}

		CharacteristicGen FeedDataShiftMod(DataIndex si, DataIndex shiftI, float mod)
		{
			using cls = std::tuple<DataIndex, DataIndex, float>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				// p[si] = std::fmod(p.t() + shiftI, mod);
				p[std::get<0>(tuple)] = std::fmod(p.t() + std::get<1>(tuple), std::get<2>(tuple));
			}, cls(si, shiftI, mod)), std::max(si, shiftI) + 1);
		}

		CharacteristicGen FeedDataTime(DataIndex si)
		{
			return Characterize(make_functor_ptr<true>([](Particle& p, DataIndex si) -> void {
				p[si] = p.t();
			}, si), si + 1);
		}

		CharacteristicGen OperateData(DataIndex si, FunctorPtr<void, float&>&& func)
		{
			using cls = std::tuple<DataIndex, FunctorPtr<void, float&>>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				// func(p[si]);
				std::get<1>(tuple)(p[std::get<0>(tuple)]);
			}, cls(si, std::move(func))), si + 1);
		}

		CharacteristicGen OperateData(DataIndex si, FunctorPtr<void, const std::tuple<float&, float>&>&& func, DataIndex di)
		{
			using cls = std::tuple<DataIndex, FunctorPtr<void, const std::tuple<float&, float>&>, DataIndex>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				// func({ p[si], p[di] });
				std::get<1>(tuple)({ p[std::get<0>(tuple)], p[std::get<2>(tuple)] });
			}, cls(si, std::move(func), di)), std::max(si, di) + 1);
		}

		CharacteristicGen OperateDataUsingSeed(DataIndex si, FunctorPtr<void, const std::tuple<float&, const CHRSeed&>&>&& func)
		{
			using cls = std::tuple<FunctorPtr<void, const std::tuple<float&, const CHRSeed&>&>, DataIndex>;
			return Characterize(make_functor_ptr<false>([](const std::tuple<Particle&, const CHRSeed&>& tuple, const cls& c) -> void {
				// func({ p[si], seed });
				std::get<0>(c)({ std::get<0>(tuple)[std::get<1>(c)], std::get<1>(tuple) });
			}, cls(std::move(func), si)), si + 1);
		}

		CharacteristicGen SetColorUsingData(FunctorPtr<Modulate, float>&& color, DataIndex di)
		{
			using cls = std::tuple<DataIndex, FunctorPtr<glm::vec4, float>>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				// set_ptr(p.m_Shape->Fickler().Modulate(), color(p[di]));
				*p.m_Shape->Fickler().Modulate() = std::get<1>(tuple)(p[std::get<0>(tuple)]);
			}, cls(di, std::move(color))), di + 1);
		}

		CharacteristicGen SetColorUsingTime(FunctorPtr<Modulate, float>&& color)
		{
			using cls = FunctorPtr<glm::vec4, float>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& color) -> void {
				*p.m_Shape->Fickler().Modulate() = color(p.t());
			}, std::move(color)), 0);
		}

		CharacteristicGen SetLocalScaleUsingData(FunctorPtr<Scale2D, float>&& scale, DataIndex di)
		{
			using cls = std::tuple<FunctorPtr<glm::vec2, float>, DataIndex>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				*p.m_Shape->Fickler().Scale() = std::get<0>(tuple)(p[std::get<1>(tuple)]);
			}, cls(std::move(scale), di)), di + 1);
		}

		CharacteristicGen SetLocalPosition(const Position2D& position)
		{
			return Characterize(make_functor_ptr<false>([](Particle& p, const Position2D& position) -> void {
				*p.m_Shape->Fickler().Position() = position;
			}, position), 0);
		}

		CharacteristicGen SetLocalPositionUsingData(FunctorPtr<Position2D, float>&& position, DataIndex di)
		{
			using cls = std::tuple<FunctorPtr<glm::vec2, float>, DataIndex>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				*p.m_Shape->Fickler().Position() = std::get<0>(tuple)(p[std::get<1>(tuple)]);
			}, cls(std::move(position), di)), di + 1);
		}

		CharacteristicGen SetLocalPositionUsingData(FunctorPtr<Position2D, const glm::vec2&>&& position, DataIndex dix, DataIndex diy)
		{
			using cls = std::tuple<FunctorPtr<glm::vec2, const glm::vec2&>, DataIndex, DataIndex>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				*p.m_Shape->Fickler().Position() = std::get<0>(tuple)(glm::vec2{ p[std::get<1>(tuple)], p[std::get<2>(tuple)] });
			}, cls(std::move(position), dix, diy)), std::max(dix, diy) + 1);
		}

		CharacteristicGen OperateLocalPositionFromVelocityData(DataIndex dix, DataIndex diy)
		{
			using cls = std::tuple<DataIndex, DataIndex>;
			return Characterize(make_functor_ptr<false>([](Particle& p, const cls& tuple) -> void {
				if (auto posref = p.m_Shape->Fickler().Position())
					*posref += glm::vec2{ p[std::get<0>(tuple)], p[std::get<1>(tuple)] } * p.dt();
			}, cls(dix, diy)), std::max(dix, diy) + 1);
		}

		static CharacteristicGen _SyncAll = Characterize(make_functor_ptr([](Particle& p) -> void { p.m_Shape->Fickler().SyncAll(); }), 0);
		static CharacteristicGen _SyncT = Characterize(make_functor_ptr([](Particle& p) -> void { p.m_Shape->Fickler().SyncT(); }), 0);
		static CharacteristicGen _SyncP = Characterize(make_functor_ptr([](Particle& p) -> void { p.m_Shape->Fickler().SyncP(); }), 0);
		static CharacteristicGen _SyncRS = Characterize(make_functor_ptr([](Particle& p) -> void { p.m_Shape->Fickler().SyncRS(); }), 0);
		static CharacteristicGen _SyncM = Characterize(make_functor_ptr([](Particle& p) -> void { p.m_Shape->Fickler().SyncM(); }), 0);

		CharacteristicGen SyncAll() { return _SyncAll.Clone(); }
		CharacteristicGen SyncT() { return _SyncT.Clone(); }
		CharacteristicGen SyncP() { return _SyncP.Clone(); }
		CharacteristicGen SyncRS() { return _SyncRS.Clone(); }
		CharacteristicGen SyncM() { return _SyncM.Clone(); }

		FunctorPtr<float, const CHRSeed&> Seed_waveT()
		{
			return make_functor_ptr([](const Particles::CHRSeed& seed) -> float { return seed.waveT; });
		}

	}

}

