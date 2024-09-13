#pragma once

#include <exception>
#include <string>
#include <vector>
#include <memory>

#include "Transform.h"
#include "Modulate.h"
#include "utils/CopyPtr.inl"
#include "utils/Meta.inl"

class Transformable2D
{
protected:
	Transformer2D* ptr = nullptr;

public:
	Transformable2D() {}
	Transformable2D(const Transformer2D& fickler) : ptr(new Transformer2D(fickler)) {}
	Transformable2D(Transformer2D&& fickler) : ptr(new Transformer2D(std::move(fickler))) {}
	Transformable2D(const Transformable2D& other)
	{
		if (other.ptr)
			ptr = new Transformer2D(*other.ptr);
	}
	Transformable2D(Transformable2D&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
private:
	friend struct Fickler2D;
	Transformable2D& operator=(const Transformable2D& other)
	{
		if (this == &other)
			return *this;
		if (other.ptr)
		{
			if (ptr)
			{
				ptr->~Transformer2D();
				new (ptr) Transformer2D(*other.ptr);
			}
			else
			{
				ptr = new Transformer2D(*other.ptr);
			}
		}
		else if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
		return *this;
	}
	Transformable2D& operator=(Transformable2D&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (ptr)
			delete ptr;
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}
public:
	~Transformable2D()
	{
		if (ptr)
			delete ptr;
	}
	operator bool() const
	{
		return ptr;
	}
	void Disengage()
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
	void Engage(const Transformer2D& fickler)
	{
		if (ptr)
		{
			ptr->~Transformer2D();
			new (ptr) Transformer2D(fickler);
		}
		else
		{
			ptr = new Transformer2D(fickler);
		}
	}
	void Engage(Transformer2D&& fickler)
	{
		if (ptr)
		{
			ptr->~Transformer2D();
			new (ptr) Transformer2D(std::move(fickler));
		}
		else
		{
			ptr = new Transformer2D(std::move(fickler));
		}
	}
	void Emplace(const Transform2D& transform = {})
	{
		if (ptr)
		{
			ptr->~Transformer2D();
			new (ptr) Transformer2D(transform);
		}
		else
		{
			ptr = new Transformer2D(transform);
		}
	}
	Transformer2D* Ref()
	{
		return ptr;
	}
	Transformer2D& operator*() { return *ptr; }
	const Transformer2D& operator*() const { return *ptr; }
	Transformer2D* operator->() { return ptr; }
};

class Modulatable
{
protected:
	Modulator* ptr = nullptr;

public:
	Modulatable() {}
	Modulatable(const Modulator& fickler) : ptr(new Modulator(fickler)) {}
	Modulatable(Modulator&& fickler) : ptr(new Modulator(std::move(fickler))) {}
	Modulatable(const Modulatable& other)
	{
		if (other.ptr)
			ptr = new Modulator(*other.ptr);
	}
	Modulatable(Modulatable&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
private:
	friend struct Fickler2D;
	Modulatable& operator=(const Modulatable& other)
	{
		if (this == &other)
			return *this;
		if (other.ptr)
		{
			if (ptr)
			{
				ptr->~Modulator();
				new (ptr) Modulator(*other.ptr);
			}
			else
			{
				ptr = new Modulator(*other.ptr);
			}
		}
		else if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
		return *this;
	}
	Modulatable& operator=(Modulatable&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (ptr)
			delete ptr;
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}
public:
	~Modulatable()
	{
		if (ptr)
			delete ptr;
	}
	operator bool() const
	{
		return ptr;
	}
	void Disengage()
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
	void Engage(const Modulator& fickler)
	{
		if (ptr)
		{
			ptr->~Modulator();
			new (ptr) Modulator(fickler);
		}
		else
		{
			ptr = new Modulator(fickler);
		}
	}
	void Engage(Modulator&& fickler)
	{
		if (ptr)
		{
			ptr->~Modulator();
			new (ptr) Modulator(std::move(fickler));
		}
		else
		{
			ptr = new Modulator(std::move(fickler));
		}
	}
	void Emplace(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f })
	{
		if (ptr)
		{
			ptr->~Modulator();
			new (ptr) Modulator(modulate);
		}
		else
		{
			ptr = new Modulator(modulate);
		}
	}
	Modulator* Ref()
	{
		return ptr;
	}
	Modulator& operator*() { return *ptr; }
	const Modulator& operator*() const { return *ptr; }
	Modulator* operator->() { return ptr; }
};

struct bad_fickle_type_error : std::exception
{
	bad_fickle_type_error() : std::exception("Tried to construct/access empty fickle.") {}
};

struct FickleType
{
	bool transformable;
	bool modulatable;
	constexpr FickleType(bool transformable, bool modulatable) : transformable(transformable), modulatable(modulatable) {}

	constexpr operator int() const
	{
		if (transformable)
			return modulatable ? 0 : 1;
		else
			return modulatable ? 2 : throw bad_fickle_type_error();
	}

	static const FickleType Protean;
	static const FickleType Transformable;
	static const FickleType Modulatable;
};

inline constexpr FickleType FickleType::Protean{ true, true };
inline constexpr FickleType FickleType::Transformable{ true, false };
inline constexpr FickleType FickleType::Modulatable{ false, true };

class ProteanChildren
{
	std::vector<Transformer2D*>& t_children;
	std::vector<Modulator*>& m_children;

public:
	ProteanChildren(std::vector<Transformer2D*>& t_children, std::vector<Modulator*>& m_children) : t_children(t_children), m_children(m_children) {}

	template<typename _Fickle>
	auto at(size_t);
	template<>
	auto at<Transformer2D>(size_t i) { return static_cast<Transformer2D*&>(t_children[i]); }
	template<>
	auto at<Modulator>(size_t i) { return static_cast<Modulator*&>(m_children[i]); }

	void push_back(Transformer2D* transformer, Modulator* modulator) { t_children.push_back(transformer); m_children.push_back(modulator); }
	void erase(size_t i) { t_children.erase(t_children.begin() + i); m_children.erase(m_children.begin() + i); }
	size_t size() const { return t_children.size(); }
};

class ProteanLinker2D
{
	Transformer2D& transformer;
	Modulator& modulator;

public:
	ProteanLinker2D(Transformer2D& transformer, Modulator& modulator) : transformer(transformer), modulator(modulator) {}

	ProteanChildren Children() { return ProteanChildren(transformer.children, modulator.children); }

	PackedP2D& PackedP() { return transformer.self.packedP; }
	PackedRS2D& PackedRS() { return transformer.self.packedRS; }
	::Modulate& PackedM() { return modulator.self.packedM; }

	void SyncAll() { transformer.Sync(); modulator.Sync(); }
	void SyncT() { transformer.Sync(); }
	void SyncP() { transformer.SyncP(); }
	void SyncRS() { transformer.SyncRS(); }
	void SyncM() { modulator.Sync(); }

	void Sync(const PackedTransform2D& pt) { transformer.self.Sync(pt); }
	void Sync(const PackedP2D& pp, const PackedRS2D& prs) { transformer.self.Sync(pp, prs); }
	void Sync(const PackedModulate& pm) { modulator.self.Sync(pm); }
	void SyncAll(const ProteanLinker2D& linker) { Sync(linker.transformer.self.packedP, linker.transformer.self.packedRS);
		modulator.self.Sync(linker.modulator.self.packedM); }
	void SyncAll(ProteanChildren& children, size_t i) { Sync(children.at<Transformer2D>(i)->self); Sync(children.at<Modulator>(i)->self); }

	void SyncChildrenAll() { transformer.SyncChildren(); modulator.SyncChildren(); }
	void SyncChildrenT() { transformer.SyncChildren(); }
	void SyncChildrenP() { transformer.SyncChildrenP(); }
	void SyncChildrenRS() { transformer.SyncChildrenRS(); }
	void SyncChildrenM() { modulator.SyncChildren(); }

	void Attach(ProteanLinker2D& linker) { transformer.Attach(&linker.transformer); modulator.Attach(&linker.modulator); }
};

struct Fickler2D
{
	Transformable2D transformable;
	Modulatable modulatable;

	Fickler2D(FickleType type = { true, true })
	{
		if (type.transformable)
			transformable.Engage(Transformer2D{});
		if (type.modulatable)
			modulatable.Engage(::Modulator{});
	}

	Transform2D* Transform() { return transformable ? &transformable->Self() : nullptr; }
	Position2D* Position() { return transformable ? &transformable->Position() : nullptr; }
	Rotation2D* Rotation() { return transformable ? &transformable->Rotation() : nullptr; }
	Scale2D* Scale() { return transformable ? &transformable->Scale() : nullptr; }
	::Modulate* Modulate() { return modulatable ? &modulatable->Self() : nullptr; }

	Transformer2D* Transformer() { return transformable ? transformable.Ref() : nullptr; }
	::Modulator* Modulator() { return modulatable ? modulatable.Ref() : nullptr; }
	ProteanLinker2D ProteanLinker() { return transformable && modulatable ? ProteanLinker2D(*transformable, *modulatable) : throw bad_fickle_type_error(); }
	
	PackedP2D* PackedP() { return transformable ? &transformable->self.packedP : nullptr; }
	PackedRS2D* PackedRS() { return transformable ? &transformable->self.packedRS : nullptr; }
	::Modulate* PackedM() { return modulatable ? &modulatable->self.packedM : nullptr; }

	void SetNotification(FickleNotification* notification) { if (transformable) transformable->notify = notification; if (modulatable) modulatable->notify = notification; }

	void SyncAll() { if (transformable) transformable->Sync(); if (modulatable) modulatable->Sync(); }
	void SyncT() { if (transformable) transformable->Sync(); }
	void SyncP() { if (transformable) transformable->SyncP(); }
	void SyncRS() { if (transformable) transformable->SyncRS(); }
	void SyncM() { if (modulatable) modulatable->Sync(); }

	void Attach(Fickler2D& fickler) { if (transformable) Transformer()->Attach(fickler.Transformer());
		if (modulatable) Modulator()->Attach(fickler.Modulator()); }
	void Attach(ProteanLinker2D& linker) { try { ProteanLinker().Attach(linker); } catch (bad_fickle_type_error) {} }
	void AttachTransformer(Fickler2D& fickler) { if (transformable) Transformer()->Attach(fickler.Transformer()); }
	void AttachTransformer(Transformer2D* transformer) { if (transformable) Transformer()->Attach(transformer); }
	void AttachModulator(Fickler2D& fickler) { if (modulatable) Modulator()->Attach(fickler.Modulator()); }
	void AttachModulator(::Modulator* modulator) { if (modulatable) Modulator()->Attach(modulator); }
	void AttachUnsafe(Fickler2D& fickler) { Transformer()->Attach(fickler.Transformer()); Modulator()->Attach(fickler.Modulator()); }
	void AttachUnsafe(ProteanLinker2D& linker) { ProteanLinker().Attach(linker); }
	void AttachTransformerUnsafe(Fickler2D& fickler) { Transformer()->Attach(fickler.Transformer()); }
	void AttachTransformerUnsafe(Transformer2D* transformer) { Transformer()->Attach(transformer); }
	void AttachModulatorUnsafe(Fickler2D& fickler) { Modulator()->Attach(fickler.Modulator()); }
	void AttachModulatorUnsafe(::Modulator* modulator) { Modulator()->Attach(modulator); }

	ProteanChildren ProteanChildren() {	return transformable && modulatable ? ::ProteanChildren(Transformer()->children, Modulator()->children) : throw bad_fickle_type_error(); }

	FickleType Type() const
	{
		if (transformable)
		{
			if (modulatable)
				return FickleType::Protean;
			else
				return FickleType::Transformable;
		}
		else
		{
			if (modulatable)
				return FickleType::Modulatable;
			else
				throw bad_fickle_type_error();
		}
	}
	bool IsProteanSafe() const
	{
		return transformable && modulatable && remove_const(transformable).Ref() && remove_const(modulatable).Ref()
			&& (remove_const(transformable)->children.size() == remove_const(modulatable)->children.size());
	}
};
