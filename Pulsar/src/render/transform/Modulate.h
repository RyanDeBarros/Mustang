#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "FickleNotification.inl"

typedef glm::vec4 Modulate;

struct PackedModulate
{
	Modulate modulate = { 1.0f, 1.0f, 1.0f, 1.0f };
	Modulate packedM = { 1.0f, 1.0f, 1.0f, 1.0f };
	
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
	void Sync(const Modulate& parentM);

};

struct Modulator
{
	PackedModulate self;
	FickleNotification* notify = nullptr;
	Modulator* parent = nullptr;
	std::vector<Modulator*> children;

	explicit inline Modulator(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }, FickleNotification* notify = nullptr, bool sync = true)
		: self{ modulate }, notify(notify)
	{
		if (sync)
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

	inline Modulate& Self() { return self.modulate; }
	void SetPackedLocalOf(const Modulate& global);

	inline void SyncChildren()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncM);
		for (const auto& c : children)
			c->Sync();
	}

	inline void Sync()
	{
		if (parent)
			self.Sync(parent->self);
		else
			self.Sync();
		SyncChildren();
	}

	inline void Attach(Modulator* modulator)
	{
		if (!modulator)
			return;
		children.push_back(modulator);
		if (modulator->parent)
			modulator->parent->Detach(modulator);
		modulator->parent = this;
	}

	inline void Detach(Modulator* modulator)
	{
		if (!modulator)
			return;
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
