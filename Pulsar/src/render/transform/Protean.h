#pragma once

#include "Transform.h"
#include "Modulate.h"

struct Proteate2D
{
	Transform2D transform = {};
	Modulate modulate = { 1.0f, 1.0f, 1.0f, 1.0f };
};

struct PackedProteate2D
{
	Proteate2D proteate;
	PackedP2D packedP = { 0.0f, 0.0f };
	PackedRS2D packedRS = { 1.0f, 0.0f, 0.0f, 1.0f };
	Modulate packedM = { 1.0f, 1.0f, 1.0f, 1.0f };

	void Sync();
	void Sync(const PackedProteate2D& parent);
	void SyncT();
	void SyncT(const PackedProteate2D& parent);
	void SyncP();
	void SyncP(const PackedProteate2D& parent);
	void SyncRS();
	void SyncRS(const PackedProteate2D& parent);
	void SyncM();
	void SyncM(const PackedProteate2D& parent);
};

struct ProteateNotification
{
	virtual void Notify() {}
	virtual void NotifyT() {}
	virtual void NotifyP() {}
	virtual void NotifyRS() {}
	virtual void NotifyM() {}
};

struct ProteanLinker2D
{
	PackedProteate2D self;
	ProteateNotification* notify = nullptr;
	ProteanLinker2D* parent = nullptr;
	std::vector<ProteanLinker2D*> children;

	inline ProteanLinker2D(const Proteate2D& proteate = { {}, { 1.0f, 1.0f, 1.0f, 1.0f } }, ProteateNotification* notify = nullptr, bool sync = true)
		: self{ proteate }, notify(std::move(notify))
	{
		if (sync)
			self.Sync();
	}
	inline ProteanLinker2D(const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }, ProteateNotification* notify = nullptr, bool sync = true)
		: self{ { transform, modulate } }, notify(std::move(notify))
	{
		if (sync)
			self.Sync();
	}

	inline ProteanLinker2D(const ProteanLinker2D& transformer)
		: self(transformer.self), parent(transformer.parent), children(transformer.children)
	{
	}

	inline ProteanLinker2D(ProteanLinker2D&& transformer) noexcept
		: self(transformer.self), parent(std::move(transformer.parent)), children(std::move(transformer.children))
	{
	}

	inline ProteanLinker2D& operator=(const ProteanLinker2D& transformer)
	{
		self = transformer.self;
		parent = transformer.parent;
		children = transformer.children;
		return *this;
	}

	inline ProteanLinker2D& operator=(ProteanLinker2D&& transformer) noexcept
	{
		self = transformer.self;
		parent = std::move(transformer.parent);
		children = std::move(transformer.children);
		return *this;
	}

	inline void Sync()
	{
		if (parent)
			self.Sync(parent->self);
		else
			self.Sync();
		if (notify)
			notify->Notify();
		for (const auto& c : children)
			c->Sync();
	}
	
	inline void SyncT()
	{
		if (parent)
			self.SyncT(parent->self);
		else
			self.SyncT();
		if (notify)
			notify->NotifyT();
		for (const auto& c : children)
			c->SyncT();
	}

	inline void SyncP()
	{
		if (parent)
			self.SyncP(parent->self);
		else
			self.SyncP();
		if (notify)
			notify->NotifyP();
		for (const auto& c : children)
			c->SyncP();
	}

	inline void SyncRS()
	{
		if (parent)
			self.SyncRS(parent->self);
		else
			self.SyncRS();
		if (notify)
			notify->NotifyRS();
		for (const auto& c : children)
			c->SyncRS();
	}

	inline void SyncM()
	{
		if (parent)
			self.SyncM(parent->self);
		else
			self.SyncM();
		if (notify)
			notify->NotifyM();
		for (const auto& c : children)
			c->SyncM();
	}

	inline void Attach(ProteanLinker2D* transformer)
	{
		children.push_back(transformer);
		if (transformer->parent)
			transformer->parent->Detach(transformer);
		transformer->parent = this;
	}

	inline void Detach(ProteanLinker2D* transformer)
	{
		if (transformer->parent != this)
			return;
		auto iter = std::find(children.begin(), children.end(), transformer);
		if (iter != children.end())
		{
			children.erase(iter);
			transformer->parent = nullptr;
		}
	}

	inline void AttachAll(ProteanLinker2D* linker_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(linker_array + i);
	}

	inline void AttachAll(ProteanLinker2D** linker_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(linker_array[i]);
	}

	inline void DetachAll(ProteanLinker2D* linker_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(linker_array + i);
	}

	inline void DetachAll(ProteanLinker2D** linker_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(linker_array[i]);
	}

};
