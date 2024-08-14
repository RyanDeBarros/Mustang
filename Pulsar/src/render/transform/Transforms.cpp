#include "Transforms.h"

namespace Transforms {

	glm::mat3 ToInverseMatrix(const Transform2D& tr)
	{
		return glm::mat3(cosf(tr.rotation) / tr.scale.x, -sinf(tr.rotation) / tr.scale.x, 0, sinf(tr.rotation) / tr.scale.y, cosf(tr.rotation) / tr.scale.y, 0, -tr.position.x, -tr.position.y, 1);
	}

	Transform2D RelTo(const Transform2D& global, const Transform2D& parent)
	{
		return { (Transforms::Rotation(-parent.rotation) * (global.position - parent.position)) / parent.scale, global.rotation - parent.rotation, global.scale / parent.scale };
	}

	Transform2D AbsTo(const Transform2D& local, const Transform2D& parent)
	{
		return { parent.position + Transforms::Rotation(parent.rotation) * (parent.scale * local.position), parent.rotation + local.rotation, parent.scale * local.scale };
	}

	glm::mat2 Rotation(glm::float32 r)
	{
		return { glm::cos(r), glm::sin(r), -glm::sin(r), glm::cos(r) };
	}

}
