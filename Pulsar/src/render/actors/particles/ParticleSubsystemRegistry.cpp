#include "ParticleSubsystemRegistry.h"

void ParticleSubsystemRegistry::Register(const std::string& id, FuncPtr func)
{
	registry[id] = func;
}

ParticleSubsystemData ParticleSubsystemRegistry::Invoke(const std::string& id, const Array<float>& params)
{
	auto iter = registry.find(id);
	if (iter != registry.end())
		return iter->second(params);
	else
		throw std::runtime_error("Function not found in ParticleSubsystemRegistry");
}
