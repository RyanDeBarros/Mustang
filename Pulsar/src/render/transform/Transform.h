#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "FickleNotification.inl"

typedef glm::vec2 Position2D;
typedef glm::float32 Rotation2D;
typedef glm::vec2 Scale2D;

typedef glm::vec2 PackedP2D;
typedef glm::mat2 PackedRS2D;

struct Transform2D
{
	Position2D position = { 0.0f, 0.0f };
	Rotation2D rotation = 0.0f;
	Scale2D scale = { 1.0f, 1.0f };
};

struct PackedTransform2D
{
	Transform2D transform = {};
	PackedP2D packedP = { 0.0f, 0.0f };
	PackedRS2D packedRS = { 1.0f, 0.0f, 0.0f, 1.0f };

	Transform2D* operator*() { return &transform; }
	Transform2D* operator->() { return &transform; }

	void Sync();
	void Sync(const PackedTransform2D& parent);
	void Sync(const PackedP2D& parentP, const PackedRS2D& parentRS);
	void SyncP();
	void SyncP(const PackedTransform2D& parent);
	void SyncRS();
	void SyncRS(const PackedTransform2D& parent);
};

struct Transformer2D
{
	PackedTransform2D self;
	FickleNotification* notify = nullptr;
	Transformer2D* parent = nullptr;
	std::vector<Transformer2D*> children;

	explicit Transformer2D(const Transform2D& transform = {}, FickleNotification* notify = nullptr, bool sync = true)
		: self{ transform }, notify(std::move(notify)) 
	{
		if (sync)
			self.Sync();
	}

	Transformer2D(const Transformer2D& transformer)
		: self(transformer.self), parent(transformer.parent), children(transformer.children)
	{
	}

	Transformer2D(Transformer2D&& transformer) noexcept
		: self(transformer.self), parent(std::move(transformer.parent)), children(std::move(transformer.children))
	{
	}
	
	Transformer2D& operator=(const Transformer2D& transformer)
	{
		self = transformer.self;
		parent = transformer.parent;
		children = transformer.children;
		return *this;
	}
	
	Transformer2D& operator=(Transformer2D&& transformer) noexcept
	{
		self = transformer.self;
		parent = std::move(transformer.parent);
		children = std::move(transformer.children);
		return *this;
	}

	Transform2D& Self() { return self.transform; }
	Position2D& Position() { return self->position; }
	Rotation2D& Rotation() { return self->rotation; }
	Scale2D& Scale() { return self->scale; }
	void SetPackedLocalOf(const PackedP2D& globalPackedP, const PackedRS2D& globalPackedRS);
	void SetPackedLocalOf(const PackedP2D& globalPackedP);
	void SetPackedLocalOf(const PackedRS2D& globalPackedRS);

	void SyncChildren()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncT);
		for (const auto& c : children)
			c->Sync();
	}

	void SyncChildrenP()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncP);
		for (const auto& c : children)
			c->SyncP();
	}

	void SyncChildrenRS()
	{
		if (notify)
			notify->Notify(FickleSyncCode::SyncRS);
		for (const auto& c : children)
			c->SyncRS();
	}

	void Sync()
	{
		if (parent)
			self.Sync(parent->self);
		else
			self.Sync();
		SyncChildren();
	}

	void SyncP()
	{
		if (parent)
			self.SyncP(parent->self);
		else
			self.SyncP();
		SyncChildrenP();
	}

	void SyncRS()
	{
		if (parent)
			self.SyncRS(parent->self);
		else
			self.SyncRS();
		SyncChildrenRS();
	}

	// use inheritance to define parent transformer that doesn't use vector of children, or possibly parent. that way, one can attach simpler transforms/modulates, such as in ActorTesselation.

	void Attach(Transformer2D* transformer)
	{
		if (!transformer)
			return;
		children.push_back(transformer);
		if (transformer->parent)
			transformer->parent->Detach(transformer);
		transformer->parent = this;
	}

	void Detach(Transformer2D* transformer)
	{
		if (!transformer)
			return;
		if (transformer->parent != this)
			return;
		auto iter = std::find(children.begin(), children.end(), transformer);
		if (iter != children.end())
		{
			children.erase(iter);
			transformer->parent = nullptr;
		}
	}

	void AttachAll(Transformer2D* transformer_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(transformer_array + i);
	}

	void AttachAll(Transformer2D** transformer_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(transformer_array[i]);
	}

	void DetachAll(Transformer2D* transformer_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(transformer_array + i);
	}

	void DetachAll(Transformer2D** transformer_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(transformer_array[i]);
	}

};
