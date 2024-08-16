#include "Transform.h"

void PackedTransform2D::Sync()
{
	auto cos = glm::cos(transform.rotation), sin = glm::sin(transform.rotation);
	packedRS = { transform.scale.x * cos, transform.scale.x * sin, -transform.scale.y * sin, transform.scale.y * cos };
	packedP = transform.position;
}

void PackedTransform2D::Sync(const PackedTransform2D& parent)
{
	auto cos = glm::cos(transform.rotation), sin = glm::sin(transform.rotation);
	packedRS = { transform.scale.x * cos, transform.scale.x * sin, -transform.scale.y * sin, transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
	packedP = parent.packedP + parent.packedRS * transform.position;
}

void PackedTransform2D::Sync(const PackedP2D& parentP, const PackedRS2D& parentRS)
{
	auto cos = glm::cos(transform.rotation), sin = glm::sin(transform.rotation);
	packedRS = { transform.scale.x * cos, transform.scale.x * sin, -transform.scale.y * sin, transform.scale.y * cos };
	packedRS = parentRS * packedRS;
	packedP = parentP + parentRS * transform.position;
}

void PackedTransform2D::SyncP()
{
	packedP = transform.position;
}

void PackedTransform2D::SyncP(const PackedTransform2D& parent)
{
	packedP = parent.packedP + parent.packedRS * transform.position;
}

void PackedTransform2D::SyncRS()
{
	auto cos = glm::cos(transform.rotation), sin = glm::sin(transform.rotation);
	packedRS = { transform.scale.x * cos, transform.scale.x * sin, -transform.scale.y * sin, transform.scale.y * cos };
}

void PackedTransform2D::SyncRS(const PackedTransform2D& parent)
{
	auto cos = glm::cos(transform.rotation), sin = glm::sin(transform.rotation);
	packedRS = { transform.scale.x * cos, transform.scale.x * sin, -transform.scale.y * sin, transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
}

void Transformer2D::SetPackedLocalOf(const PackedP2D& globalPackedP, const PackedRS2D& globalPackedRS)
{
	self.packedRS = globalPackedRS / parent->self.packedRS;
	self.packedP = (globalPackedP - parent->self.packedP) / parent->self.packedRS;
}

void Transformer2D::SetPackedLocalOf(const PackedP2D& globalPackedP)
{
	self.packedP = (globalPackedP - parent->self.packedP) / parent->self.packedRS;
}

void Transformer2D::SetPackedLocalOf(const PackedRS2D& globalPackedRS)
{
	self.packedRS = globalPackedRS / parent->self.packedRS;
}
