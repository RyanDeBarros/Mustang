#pragma once

#include <unordered_map>

#include "ParticleSubsystem.h"
#include "utils/Array.inl"

struct psregistry_mismatch_error : public std::runtime_error
{
	inline psregistry_mismatch_error() : std::runtime_error("array size mismatch") {}
};

class ParticleSubsystemRegistry
{
	using FuncPtr = ParticleSubsystemData(*)(const Array<float>&);

	std::unordered_map<std::string, FuncPtr> registry;

	ParticleSubsystemRegistry() = default;

public:
	inline static ParticleSubsystemRegistry& Instance()
	{
		static ParticleSubsystemRegistry instance;
		return instance;
	}

	void Register(const std::string& id, FuncPtr func);
	ParticleSubsystemData Invoke(const std::string& id, const Array<float>& params);

};
