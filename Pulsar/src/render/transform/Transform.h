#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

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

	inline Transform2D* operator*()
	{
		return &transform;
	}
	inline Transform2D* operator->()
	{
		return &transform;
	}

	void Sync();
	void Sync(const PackedTransform2D& parent);
	void SyncP();
	void SyncP(const PackedTransform2D& parent);
	void SyncRS();
	void SyncRS(const PackedTransform2D& parent);
};

struct TransformNotification
{
	virtual void Notify() {}
	virtual void NotifyP() {}
	virtual void NotifyRS() {}
};

struct Transformer2D
{
	PackedTransform2D self;
	std::unique_ptr<TransformNotification> notify = nullptr;
	Transformer2D* parent = nullptr;
	std::vector<Transformer2D*> children;

	inline Transformer2D(const Transform2D& transform = {}, std::unique_ptr<TransformNotification>&& notify = {})
		: self{ transform }, notify(std::move(notify)) 
	{
		self.Sync();
	}

	inline Transformer2D(const Transformer2D& transformer)
		: self(transformer.self), parent(transformer.parent), children(transformer.children)
	{
	}

	inline Transformer2D(Transformer2D&& transformer) noexcept
		: self(transformer.self), parent(std::move(transformer.parent)), children(std::move(transformer.children))
	{
	}
	
	inline Transformer2D& operator=(const Transformer2D& transformer)
	{
		self = transformer.self;
		parent = transformer.parent;
		children = transformer.children;
		return *this;
	}
	
	inline Transformer2D& operator=(Transformer2D&& transformer) noexcept
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

	inline void Attach(Transformer2D* transformer)
	{
		children.push_back(transformer);
		if (transformer->parent)
			transformer->parent->Detach(transformer);
		transformer->parent = this;
	}

	inline void Detach(Transformer2D* transformer)
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

	inline void AttachAll(Transformer2D* transformer_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(transformer_array + i);
	}

	inline void AttachAll(Transformer2D** transformer_array, size_t length)
	{
		children.reserve(children.capacity() + length);
		for (size_t i = 0; i < length; i++)
			Attach(transformer_array[i]);
	}

	inline void DetachAll(Transformer2D* transformer_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(transformer_array + i);
	}

	inline void DetachAll(Transformer2D** transformer_array, size_t length)
	{
		for (size_t i = 0; i < length; i++)
			Detach(transformer_array[i]);
	}

};
