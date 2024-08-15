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
