#include "Modulate.h"

void PackedModulate::Sync()
{
	packedM = modulate;
}

void PackedModulate::Sync(const PackedModulate& parent)
{
	packedM = modulate * parent.packedM;
}

void PackedModulate::Sync(const Modulate& parentM)
{
	packedM = modulate * parentM;
}

void Modulator::SetPackedLocalOf(const Modulate& global)
{
	self.packedM = global / parent->self.packedM;
}
