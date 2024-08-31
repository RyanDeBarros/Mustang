#pragma once

#include <tuple>

#include "Particle.h"

namespace Particles {

	extern CharacteristicGen Characterize(CHRFunc&& f, DataIndex max_index);
	extern CharacteristicGen Characterize(Functor<void, const std::tuple<Particle&, const CHRSeed&>&>&& f, DataIndex max_index);

	extern CharacteristicGen CombineSequential(std::vector<CharacteristicGen>&& characteristics);
	extern CharacteristicGen GenSetup(CharacteristicGen&& setup);
	extern CharacteristicGen CombineInitialOverTime(CharacteristicGen&& initial, CharacteristicGen&& over_time);
	extern CharacteristicGen CombineIntervals(CharacteristicGen&& first, CharacteristicGen&& second, float divider, bool or_equal = false);
	extern CharacteristicGen CombineConditionalDataLessThan(DataIndex di, float compared_to, CharacteristicGen&& first, CharacteristicGen&& second);
	extern CharacteristicGen CombineConditionalTimeLessThan(float compared_to, CharacteristicGen&& first, CharacteristicGen&& second);

	namespace CHR {

		extern CharacteristicGen FeedDataRNG(DataIndex si);
		extern CharacteristicGen FeedData(DataIndex si, float v);
		extern CharacteristicGen FeedDataRandSpawnIndex(DataIndex si);
		extern CharacteristicGen FeedDataRandBinChoice(DataIndex si, float option1, float option2);
		extern CharacteristicGen FeedDataShiftMod(DataIndex si, DataIndex shiftI, float mod);
		extern CharacteristicGen FeedDataTime(DataIndex si);

		extern CharacteristicGen OperateData(DataIndex si, Functor<void, float&>&& func);
		extern CharacteristicGen OperateData(DataIndex si, Functor<void, const std::tuple<float&, float>&>&& func, DataIndex di);
		extern CharacteristicGen OperateDataUsingSeed(DataIndex si, Functor<void, const std::tuple<float&, const CHRSeed&>&>&& func);

		extern CharacteristicGen SetColorUsingData(Functor<Modulate, float>&& color, DataIndex di);
		extern CharacteristicGen SetColorUsingTime(Functor<Modulate, float>&& color);
		
		extern CharacteristicGen SetLocalScaleUsingData(Functor<Scale2D, float>&& scale, DataIndex di);
		
		extern CharacteristicGen SetLocalPosition(const Position2D& position);
		extern CharacteristicGen SetLocalPositionUsingData(Functor<Position2D, float>&& position, DataIndex di);
		extern CharacteristicGen SetLocalPositionUsingData(Functor<Position2D, const glm::vec2&>&& position, DataIndex dix, DataIndex diy);
		extern CharacteristicGen OperateLocalPositionFromVelocityData(DataIndex dix, DataIndex diy);

		extern CharacteristicGen SyncAll();
		extern CharacteristicGen SyncT();
		extern CharacteristicGen SyncP();
		extern CharacteristicGen SyncRS();
		extern CharacteristicGen SyncM();

		extern Functor<float, const CHRSeed&> Seed_waveT();
	}

}
