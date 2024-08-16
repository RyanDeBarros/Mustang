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
	void SyncT(const PackedTransform2D& parent);
	void SyncP();
	void SyncP(const PackedProteate2D& parent);
	void SyncRS();
	void SyncRS(const PackedProteate2D& parent);
	void SyncM();
	void SyncM(const PackedProteate2D& parent);
	void SyncM(const PackedModulate& parent);
};

struct ProteanLinker2D
{
	PackedProteate2D self;
	FickleNotification* notify = nullptr;
	ProteanLinker2D* parent = nullptr;
	std::vector<ProteanLinker2D*> children;

	explicit inline ProteanLinker2D(const Proteate2D& proteate = { {}, { 1.0f, 1.0f, 1.0f, 1.0f } }, FickleNotification* notify = nullptr, bool sync = true)
		: self{ proteate }, notify(std::move(notify))
	{
		if (sync)
			self.Sync();
	}
	inline ProteanLinker2D(const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f }, FickleNotification* notify = nullptr, bool sync = true)
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

	inline Transform2D& Transform() { return self.proteate.transform; }
	inline Position2D& Position() { return self.proteate.transform.position; }
	inline Rotation2D& Rotation() { return self.proteate.transform.rotation; }
	inline Scale2D& Scale() { return self.proteate.transform.scale; }
	inline Modulate& Modulate() { return self.proteate.modulate; }

	void SetPackedLocalOf(const PackedP2D& globalPackedP, const PackedRS2D& globalPackedRS);
	void SetPackedLocalOf(const PackedP2D& globalPackedP);
	void SetPackedLocalOf(const PackedRS2D& globalPackedRS);
	void SetPackedLocalOf(const ::Modulate& globalModulate);

	inline void SyncChildren()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncAll);
		for (const auto& c : children)
			c->Sync();
	}

	inline void SyncChildrenT()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncT);
		for (const auto& c : children)
			c->SyncT();
	}

	inline void SyncChildrenP()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncP);
		for (const auto& c : children)
			c->SyncP();
	}

	inline void SyncChildrenRS()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncRS);
		for (const auto& c : children)
			c->SyncRS();
	}

	inline void SyncChildrenM()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncM);
		for (const auto& c : children)
			c->SyncM();
	}

	inline void Sync()
	{
		if (parent)
			self.Sync(parent->self);
		else
			self.Sync();
		SyncChildren();
	}
	
	inline void SyncT()
	{
		if (parent)
			self.SyncT(parent->self);
		else
			self.SyncT();
		SyncChildrenT();
	}

	inline void SyncP()
	{
		if (parent)
			self.SyncP(parent->self);
		else
			self.SyncP();
		SyncChildrenP();
	}

	inline void SyncRS()
	{
		if (parent)
			self.SyncRS(parent->self);
		else
			self.SyncRS();
		SyncChildrenRS();
	}

	inline void SyncM()
	{
		if (parent)
			self.SyncM(parent->self);
		else
			self.SyncM();
		SyncChildrenM();
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
