#include "Transform.h"

static inline glm::vec2 operator/(const float& f, const glm::vec2& v)
{
	return glm::vec2(1 / v.x, 1 / v.y);
}

namespace Transform {
	
	inline glm::mat3 ToMatrix(const Transform2D& tr)
	{
		return glm::mat3(tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), 0, -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation), 0, tr.position.x, tr.position.y, 1);
	}

	inline glm::mat3 ToInverseMatrix(const Transform2D& tr)
	{
		return glm::mat3(cosf(tr.rotation) / tr.scale.x, -sinf(tr.rotation) / tr.scale.x, 0, sinf(tr.rotation) / tr.scale.y, cosf(tr.rotation) / tr.scale.y, 0, -tr.position.x, -tr.position.y, 1);
	}

	inline glm::mat3x2 ToCondensedMatrix(const Transform2D& tr)
	{
		return glm::mat3x2(tr.position.x, tr.position.y, tr.scale.x * cosf(tr.rotation), tr.scale.x * sinf(tr.rotation), -tr.scale.y * sinf(tr.rotation), tr.scale.y * cosf(tr.rotation));
	}
	
	inline Transform2D Inverse(const Transform2D& tr)
	{
		return { -tr.position, -tr.rotation, 1 / tr.scale };
	}

}

Transform2D Transform2D::operator^(const Transform2D& transform) const
{
	return { position + transform.position, rotation + transform.rotation, scale * transform.scale };
}
