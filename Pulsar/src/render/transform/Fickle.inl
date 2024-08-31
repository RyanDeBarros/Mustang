#pragma once

#include <exception>
#include <string>
#include <vector>
#include <memory>

#include "Transform.h"
#include "Modulate.h"
#include "utils/CopyPtr.inl"

// TODO put somewhere else?
template<typename T>
inline T& remove_const(const T& el)
{
	return *const_cast<T*>(&el);
}

template<typename T>
class FickleWrapper
{
protected:
	T* ptr = nullptr;

public:
	inline FickleWrapper() {}
	inline FickleWrapper(const T& fickler) : ptr(new T(fickler)) {}
	inline FickleWrapper(T&& fickler) : ptr(new T(std::move(fickler))) {}
	inline FickleWrapper(const FickleWrapper<T>& other)
	{
		if (other.ptr)
			ptr = new T(*other.ptr);
	}
	inline FickleWrapper(FickleWrapper<T>&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
protected:
	friend struct Fickler2D;
	inline FickleWrapper& operator=(const FickleWrapper<T>& other)
	{
		if (this == &other)
			return *this;
		if (other.ptr)
		{
			if (ptr)
			{
				ptr->~T();
				new (ptr) T(*other.ptr);
			}
			else
			{
				ptr = new T(*other.ptr);
			}
		}
		else if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
		return *this;
	}
	inline FickleWrapper& operator=(FickleWrapper<T>&& other) noexcept
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
	~FickleWrapper()
	{
		if (ptr)
			delete ptr;
	}
	inline operator bool() const
	{
		return ptr;
	}
	inline void Disengage()
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
	inline void Engage(const T& fickler)
	{
		if (ptr)
		{
			ptr->~T();
			new (ptr) T(fickler);
		}
		else
		{
			ptr = new T(fickler);
		}
	}
	inline void Engage(T&& fickler)
	{
		if (ptr)
		{
			ptr->~T();
			new (ptr) T(std::move(fickler));
		}
		else
		{
			ptr = new T(std::move(fickler));
		}
	}
	inline T* Ref()
	{
		return ptr;
	}
	inline T& operator*() { return *ptr; }
	inline const T& operator*() const { return *ptr; }
	inline T* operator->() { return ptr; }
};

struct Transformable2D : public FickleWrapper<Transformer2D>
{
	inline Transformable2D() : FickleWrapper<Transformer2D>() {}
	inline Transformable2D(const Transformer2D& transformer) : FickleWrapper<Transformer2D>(transformer) {}
	inline Transformable2D(Transformer2D&& transformer) : FickleWrapper<Transformer2D>(std::move(transformer)) {}
	
	inline void Emplace(const Transform2D& transform = {})
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
};

struct Modulatable : public FickleWrapper<Modulator>
{
	inline Modulatable() : FickleWrapper<Modulator>() {}
	inline Modulatable(const Modulator& modulator) : FickleWrapper<Modulator>(modulator) {}
	inline Modulatable(Modulator&& modulator) : FickleWrapper<Modulator>(std::move(modulator)) {}

	inline void Emplace(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f })
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
};

struct bad_fickle_type_error : std::exception
{
	bad_fickle_type_error() : std::exception("Tried to construct/access empty fickle.") {}
};

struct FickleType
{
	bool transformable;
	bool modulatable;
	inline constexpr FickleType(bool transformable, bool modulatable) : transformable(transformable), modulatable(modulatable) {}

	inline constexpr operator int() const
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
	inline ProteanChildren(std::vector<Transformer2D*>& t_children, std::vector<Modulator*>& m_children) : t_children(t_children), m_children(m_children) {}

	template<typename _Fickle>
	auto at(size_t);
	template<>
	inline auto at<Transformer2D>(size_t i) { return static_cast<Transformer2D*&>(t_children[i]); }
	template<>
	inline auto at<Modulator>(size_t i) { return static_cast<Modulator*&>(m_children[i]); }

	void push_back(Transformer2D* transformer, Modulator* modulator) { t_children.push_back(transformer); m_children.push_back(modulator); }
	void erase(size_t i) { t_children.erase(t_children.begin() + i); m_children.erase(m_children.begin() + i); }
	size_t size() const { return t_children.size(); }
};

class ProteanLinker2D
{
	Transformer2D& transformer;
	Modulator& modulator;

public:
	inline ProteanLinker2D(Transformer2D& transformer, Modulator& modulator) : transformer(transformer), modulator(modulator) {}

	ProteanChildren Children() { return ProteanChildren(transformer.children, modulator.children); }

	inline PackedP2D& PackedP() { return transformer.self.packedP; }
	inline PackedRS2D& PackedRS() { return transformer.self.packedRS; }
	inline ::Modulate& PackedM() { return modulator.self.packedM; }

	inline void SyncAll() { transformer.Sync(); modulator.Sync(); }
	inline void SyncT() { transformer.Sync(); }
	inline void SyncP() { transformer.SyncP(); }
	inline void SyncRS() { transformer.SyncRS(); }
	inline void SyncM() { modulator.Sync(); }

	inline void Sync(const PackedTransform2D& pt) { transformer.self.Sync(pt); }
	inline void Sync(const PackedP2D& pp, const PackedRS2D& prs) { transformer.self.Sync(pp, prs); }
	inline void Sync(const PackedModulate& pm) { modulator.self.Sync(pm); }
	inline void SyncAll(const ProteanLinker2D& linker) { Sync(linker.transformer.self.packedP, linker.transformer.self.packedRS);
		modulator.self.Sync(linker.modulator.self.packedM); }
	inline void SyncAll(ProteanChildren& children, size_t i) { Sync(children.at<Transformer2D>(i)->self); Sync(children.at<Modulator>(i)->self); }

	inline void SyncChildrenAll() { transformer.SyncChildren(); modulator.SyncChildren(); }
	inline void SyncChildrenT() { transformer.SyncChildren(); }
	inline void SyncChildrenP() { transformer.SyncChildrenP(); }
	inline void SyncChildrenRS() { transformer.SyncChildrenRS(); }
	inline void SyncChildrenM() { modulator.SyncChildren(); }

	inline void Attach(ProteanLinker2D& linker) { transformer.Attach(&linker.transformer); modulator.Attach(&linker.modulator); }
};

struct Fickler2D
{
	Transformable2D transformable;
	Modulatable modulatable;

	inline Fickler2D(FickleType type = { true, true })
	{
		if (type.transformable)
			transformable.Engage(Transformer2D{});
		if (type.modulatable)
			modulatable.Engage(::Modulator{});
	}

	inline Transform2D* Transform() { return transformable ? &transformable->Self() : nullptr; }
	inline Position2D* Position() { return transformable ? &transformable->Position() : nullptr; }
	inline Rotation2D* Rotation() { return transformable ? &transformable->Rotation() : nullptr; }
	inline Scale2D* Scale() { return transformable ? &transformable->Scale() : nullptr; }
	inline ::Modulate* Modulate() { return modulatable ? &modulatable->Self() : nullptr; }

	inline Transformer2D* Transformer() { return transformable ? transformable.Ref() : nullptr; }
	inline ::Modulator* Modulator() { return modulatable ? modulatable.Ref() : nullptr; }
	inline ProteanLinker2D ProteanLinker() { return transformable && modulatable ? ProteanLinker2D(*transformable, *modulatable) : throw bad_fickle_type_error(); }
	
	inline PackedP2D* PackedP() { return transformable ? &transformable->self.packedP : nullptr; }
	inline PackedRS2D* PackedRS() { return transformable ? &transformable->self.packedRS : nullptr; }
	inline ::Modulate* PackedM() { return modulatable ? &modulatable->self.packedM : nullptr; }

	inline void SetNotification(FickleNotification* notification) { if (transformable) transformable->notify = notification; if (modulatable) modulatable->notify = notification; }

	inline void SyncAll() { if (transformable) transformable->Sync(); if (modulatable) modulatable->Sync(); }
	inline void SyncT() { if (transformable) transformable->Sync(); }
	inline void SyncP() { if (transformable) transformable->SyncP(); }
	inline void SyncRS() { if (transformable) transformable->SyncRS(); }
	inline void SyncM() { if (modulatable) modulatable->Sync(); }

	inline void Attach(Fickler2D& fickler) { if (transformable) Transformer()->Attach(fickler.Transformer());
		if (modulatable) Modulator()->Attach(fickler.Modulator()); }
	inline void Attach(ProteanLinker2D& linker) { try { ProteanLinker().Attach(linker); } catch (bad_fickle_type_error) {} }
	inline void AttachTransformer(Fickler2D& fickler) { if (transformable) Transformer()->Attach(fickler.Transformer()); }
	inline void AttachTransformer(Transformer2D* transformer) { if (transformable) Transformer()->Attach(transformer); }
	inline void AttachModulator(Fickler2D& fickler) { if (modulatable) Modulator()->Attach(fickler.Modulator()); }
	inline void AttachModulator(::Modulator* modulator) { if (modulatable) Modulator()->Attach(modulator); }
	inline void AttachUnsafe(Fickler2D& fickler) { Transformer()->Attach(fickler.Transformer()); Modulator()->Attach(fickler.Modulator()); }
	inline void AttachUnsafe(ProteanLinker2D& linker) { ProteanLinker().Attach(linker); }
	inline void AttachTransformerUnsafe(Fickler2D& fickler) { Transformer()->Attach(fickler.Transformer()); }
	inline void AttachTransformerUnsafe(Transformer2D* transformer) { Transformer()->Attach(transformer); }
	inline void AttachModulatorUnsafe(Fickler2D& fickler) { Modulator()->Attach(fickler.Modulator()); }
	inline void AttachModulatorUnsafe(::Modulator* modulator) { Modulator()->Attach(modulator); }

	inline ProteanChildren ProteanChildren() {	return transformable && modulatable ? ::ProteanChildren(Transformer()->children, Modulator()->children) : throw bad_fickle_type_error(); }

	inline FickleType Type() const
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
	inline bool IsProteanSafe() const
	{
		return transformable && modulatable && remove_const(transformable).Ref() && remove_const(modulatable).Ref()
			&& (remove_const(transformable)->children.size() == remove_const(modulatable)->children.size());
	}
};
