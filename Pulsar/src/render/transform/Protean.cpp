#include "Protean.h"

void PackedProteate2D::Sync()
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedP = proteate.transform.position;
	packedM = proteate.modulate;
}

void PackedProteate2D::Sync(const PackedProteate2D& parent)
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
	packedP = parent.packedP + parent.packedRS * proteate.transform.position;
	packedM = proteate.modulate * parent.packedM;
}

void PackedProteate2D::SyncT()
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedP = proteate.transform.position;
}

void PackedProteate2D::SyncT(const PackedProteate2D& parent)
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
	packedP = parent.packedP + parent.packedRS * proteate.transform.position;
}

void PackedProteate2D::SyncT(const PackedTransform2D& parent)
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
	packedP = parent.packedP + parent.packedRS * proteate.transform.position;
}

void PackedProteate2D::SyncP()
{
	packedP = proteate.transform.position;
}

void PackedProteate2D::SyncP(const PackedProteate2D& parent)
{
	packedP = parent.packedP + parent.packedRS * proteate.transform.position;
}

void PackedProteate2D::SyncRS()
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
}

void PackedProteate2D::SyncRS(const PackedProteate2D& parent)
{
	auto cos = glm::cos(proteate.transform.rotation), sin = glm::sin(proteate.transform.rotation);
	packedRS = { proteate.transform.scale.x * cos, proteate.transform.scale.x * sin, -proteate.transform.scale.y * sin, proteate.transform.scale.y * cos };
	packedRS = parent.packedRS * packedRS;
}

void PackedProteate2D::SyncM()
{
	packedM = proteate.modulate;
}

void PackedProteate2D::SyncM(const PackedProteate2D& parent)
{
	packedM = proteate.modulate * parent.packedM;
}

void PackedProteate2D::SyncM(const PackedModulate& parent)
{
	packedM = proteate.modulate * parent.packedM;
}

void ProteanLinker2D::SetPackedLocalOf(const PackedP2D& globalPackedP, const PackedRS2D& globalPackedRS)
{
	self.packedRS = globalPackedRS / parent->self.packedRS;
	self.packedP = (globalPackedP - parent->self.packedP) / parent->self.packedRS;
}

void ProteanLinker2D::SetPackedLocalOf(const PackedP2D& globalPackedP)
{
	self.packedP = (globalPackedP - parent->self.packedP) / parent->self.packedRS;
}

void ProteanLinker2D::SetPackedLocalOf(const PackedRS2D& globalPackedRS)
{
	self.packedRS = globalPackedRS / parent->self.packedRS;
}

void ProteanLinker2D::SetPackedLocalOf(const ::Modulate& global)
{
	self.packedM = global / parent->self.packedM;
}