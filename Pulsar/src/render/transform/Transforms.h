#pragma once

#include "Transform.h"

namespace Transforms {

	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);
	extern glm::mat2 Rotation(glm::float32 r);

}
