#include "Modulate.h"

void PackedModulate::Sync()
{
	packedM = modulate;
}

void PackedModulate::Sync(const PackedModulate& parent)
{
	packedM = modulate * parent.packedM;
}
