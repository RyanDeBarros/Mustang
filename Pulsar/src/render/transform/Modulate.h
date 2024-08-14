#pragma once

#include <glm/glm.hpp>
#include <vector>

typedef glm::vec4 Modulate;

struct PackedModulate
{
	Modulate modulate;
	Modulate packedM;
	
	inline Modulate* operator*()
	{
		return &modulate;
	}
	inline Modulate* operator->()
	{
		return &modulate;
	}

	void Sync();
	void Sync(const PackedModulate& parent);

};

struct ModulateNotification
{
	virtual void NotifyM() {}
};

struct Modulator
{
	PackedModulate self;
	ModulateNotification* notify = nullptr;
	Modulator* parent = nullptr;
	std::vector<Modulator*> children;

	inline Modulator(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }, ModulateNotification* notify = nullptr)
		: self{ modulate }, notify(notify)
	{
		self.Sync();
	}

	inline Modulator(const Modulator& modulator)
		: self(modulator.self), parent(modulator.parent), children(modulator.children)
	{
	}

	inline Modulator(Modulator&& modulator) noexcept
		: self(modulator.self), parent(std::move(modulator.parent)), children(std::move(modulator.children))
	{
	}

	inline Modulator& operator=(const Modulator& modulator)
	{
		self = modulator.self;
		parent = modulator.parent;
		children = modulator.children;
		return *this;
	}

	inline Modulator& operator=(Modulator&& modulator) noexcept
	{
		self = modulator.self;
		parent = std::move(modulator.parent);
		children = std::move(modulator.children);
		return *this;
	}

	inline void Sync()
	{
		if (parent)
			self.Sync(parent->self);
		else
			self.Sync();
		if (notify)
			notify->NotifyM();
		for (const auto& c : children)
			c->Sync();
	}

	inline void Attach(Modulator* modulator)
	{
		children.push_back(modulator);
		if (modulator->parent)
			modulator->parent->Detach(modulator);
		modulator->parent = this;
	}

	inline void Detach(Modulator* modulator)
	{
		if (modulator->parent != this)
			return;
		auto iter = std::find(children.begin(), children.end(), modulator);
		if (iter != children.end())
		{
			children.erase(iter);
			modulator->parent = nullptr;
		}
	}

	inline void AttachAll(Modulator* modulator_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(modulator_array + i);
	}

	inline void AttachAll(Modulator** modulator_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(modulator_array[i]);
	}

	inline void DetachAll(Modulator* modulator_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(modulator_array + i);
	}

	inline void DetachAll(Modulator** modulator_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(modulator_array[i]);
	}
};
