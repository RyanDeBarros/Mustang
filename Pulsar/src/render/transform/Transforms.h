#pragma once

#include "Transform.h"

namespace Transforms {

	extern glm::mat3 ToInverseMatrix(const Transform2D& tr);

	extern glm::mat2 Rotation(glm::float32 r);
	extern Transform2D RelTo(const Transform2D& global, const Transform2D& parent);
	extern Transform2D AbsTo(const Transform2D& local, const Transform2D& parent);

}
