#pragma once

#include "Particle.h"

namespace Particles {

	extern CharacteristicGen CombineSequential(const std::vector<CharacteristicGen>& characteristics);
	extern CharacteristicGen GenSetup(const CharacteristicGen& setup);
	extern CharacteristicGen CombineInitialOverTime(const CharacteristicGen& initial, const CharacteristicGen& over_time);
	extern CharacteristicGen CombineIntervals(const CharacteristicGen& first, const CharacteristicGen& second, float divider, bool or_equal = false);
	extern CharacteristicGen CombineConditionalDataLessThan(DataIndex di, float compared_to, const CharacteristicGen& first, const CharacteristicGen& second);
	extern CharacteristicGen CombineConditionalTimeLessThan(float compared_to, const CharacteristicGen& first, const CharacteristicGen& second);

	namespace CHR {

		extern CharacteristicGen FeedDataRNG(DataIndex si);
		extern CharacteristicGen FeedDataRandSpawnIndex(DataIndex si);
		extern CharacteristicGen FeedDataRandBinChoice(DataIndex si, float option1, float option2);
		extern CharacteristicGen FeedDataShiftMod(DataIndex si, DataIndex shiftI, float mod);
		extern CharacteristicGen OperateData(DataIndex si, const std::function<void(float&)>& func);
		extern CharacteristicGen OperateData(DataIndex si, const std::function<std::function<void(float&)>(float)>& func, DataIndex di);
		extern CharacteristicGen OperateData(DataIndex si1, const std::function<void(float&)>& func1, DataIndex si2, const std::function<void(float&)>& func2);
		extern CharacteristicGen OperateData(DataIndex si1, const std::function<std::function<void(float&)>(float)>& func1, DataIndex di1, DataIndex si2, const std::function<std::function<void(float&)>(float)>& func2, DataIndex di2);

		extern CharacteristicGen SetColorUsingData(const std::function<glm::vec4(float)>& color, DataIndex di);
		extern CharacteristicGen SetColorUsingTime(const std::function<glm::vec4(float)>& color);
		
		extern CharacteristicGen SetLocalScaleUsingData(const std::function<glm::vec2(float)>& scale, DataIndex di);
		
		extern CharacteristicGen SetLocalPositionUsingData(const std::function<glm::vec2(float)>& position, DataIndex di);
		extern CharacteristicGen SetLocalPositionUsingData(const std::function<glm::vec2(glm::vec2)>& position, DataIndex di1, DataIndex di2);
		extern CharacteristicGen OperateLocalPositionFromVelocityData(DataIndex dix, DataIndex diy);
		
		extern CharacteristicGen SyncGlobalWithLocal;

	}

}
