#pragma once

#include <exception>
#include <string>
#include <vector>
#include <memory>

#include "Transform.h"
#include "Modulate.h"
#include "Protean.h"

template<typename T>
class FickleWrapper
{
protected:
	T* m_Ptr = nullptr;

public:
	inline FickleWrapper() {}
	inline FickleWrapper(const T& fickler) : m_Ptr(new T(fickler)) {}
	inline FickleWrapper(T&& fickler) : m_Ptr(new T(std::move(fickler))) {}
	inline FickleWrapper(const FickleWrapper<T>& other)
	{
		if (other.m_Ptr)
			m_Ptr = new T(*other.m_Ptr);
	}
	inline FickleWrapper(FickleWrapper<T>&& other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}
	inline FickleWrapper& operator=(const FickleWrapper<T>& other)
	{
		if (this == &other)
			return *this;
		if (other.m_Ptr)
		{
			if (m_Ptr)
			{
				m_Ptr->~T();
				new (m_Ptr) T(*other.m_Ptr);
			}
			else
			{
				m_Ptr = new T(*other.m_Ptr);
			}
		}
		else if (m_Ptr)
		{
			delete m_Ptr;
			m_Ptr = nullptr;
		}
		return *this;
	}
	inline FickleWrapper& operator=(FickleWrapper<T>&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (m_Ptr)
			delete m_Ptr;
		m_Ptr = other.m_Ptr;
		other.m_Ptr = nullptr;
		return *this;
	}
	~FickleWrapper()
	{
		if (m_Ptr)
			delete m_Ptr;
	}
	inline operator bool() const
	{
		return m_Ptr;
	}
	inline void Disengage()
	{
		if (m_Ptr)
		{
			delete m_Ptr;
			m_Ptr = nullptr;
		}
	}
	inline void Engage(const T& fickler)
	{
		if (m_Ptr)
		{
			m_Ptr->~T();
			new (m_Ptr) T(fickler);
		}
		else
		{
			m_Ptr = new T(fickler);
		}
	}
	inline void Engage(T&& fickler)
	{
		if (m_Ptr)
		{
			m_Ptr->~T();
			new (m_Ptr) T(std::move(fickler));
		}
		else
		{
			m_Ptr = new T(std::move(fickler));
		}
	}
	inline T* Ref()
	{
		return m_Ptr;
	}
};

struct Transformable2D : public FickleWrapper<Transformer2D>
{
	inline void Emplace(const Transform2D& transform = {})
	{
		if (m_Ptr)
		{
			m_Ptr->~Transformer2D();
			new (m_Ptr) Transformer2D(transform);
		}
		else
		{
			m_Ptr = new Transformer2D(transform);
		}
	}
};

struct Modulatable : public FickleWrapper<Modulator>
{
	inline void Emplace(const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f })
	{
		if (m_Ptr)
		{
			m_Ptr->~Modulator();
			new (m_Ptr) Modulator(modulate);
		}
		else
		{
			m_Ptr = new Modulator(modulate);
		}
	}
};

struct Protean2D : public FickleWrapper<ProteanLinker2D>
{
	inline void Emplace(const Transform2D& transform = {}, const Modulate& modulate = { 1.0f, 1.0f, 1.0f, 1.0f })
	{
		if (m_Ptr)
		{
			m_Ptr->~ProteanLinker2D();
			new (m_Ptr) ProteanLinker2D(transform, modulate);
		}
		else
		{
			m_Ptr = new ProteanLinker2D(transform, modulate);
		}
	}
};

constexpr signed char PROTEAN2D = 0;
constexpr signed char TRANSFORMABLE2D = 1;
constexpr signed char MODULATABLE = 2;

enum class FickleType : char
{
	Protean = 0,
	Transformable = 1,
	Modulatable = 2
};

struct BadFickleSelect : public std::exception
{
	BadFickleSelect(FickleType original, FickleType setter) : std::exception(("Original fickle (" + std::to_string(char(original)) + ") - Setter fickle (" + std::to_string(char(setter)) + ")").c_str()) {}
};

class FickleSelector2D
{
	union {
		Protean2D m_Protean;
		Transformable2D m_Transformable;
		Modulatable m_Modulatable;
	};
	FickleType type;

public:
	FickleSelector2D() = delete;
	inline FickleSelector2D(FickleType type)
		: type(type)
	{
		if (type == FickleType::Protean) [[likely]]
		{
			new (&m_Protean) Protean2D();
			m_Protean.Emplace();
		}
		else if (type == FickleType::Transformable)
		{
			new (&m_Transformable) Transformable2D();
			m_Transformable.Emplace();
		}
		else if (type == FickleType::Modulatable) [[unlikely]]
		{
			new (&m_Modulatable) ::Modulatable();
			m_Modulatable.Emplace();
		}
		else [[unlikely]]
		{
			__debugbreak();
		}
	}
	inline FickleSelector2D(const FickleSelector2D& other)
		: type(other.type)
	{
		if (type == FickleType::Protean) [[likely]]
		{
			new (&m_Protean) Protean2D();
			m_Protean = other.m_Protean;
		}
		else if (type == FickleType::Transformable)
		{
			new (&m_Transformable) Transformable2D();
			m_Transformable = other.m_Transformable;
		}
		else if (type == FickleType::Modulatable) [[unlikely]]
		{
			new (&m_Modulatable) ::Modulatable();
			m_Modulatable = other.m_Modulatable;
		}
	}
	inline FickleSelector2D(FickleSelector2D&& other) noexcept
		: type(other.type)
	{
		if (type == FickleType::Protean) [[likely]]
		{
			new (&m_Protean) Protean2D();
			m_Protean = std::move(other.m_Protean);
		}
		else if (type == FickleType::Transformable)
		{
			new (&m_Transformable) Transformable2D();
			m_Transformable = std::move(other.m_Transformable);
		}
		else if (type == FickleType::Modulatable) [[unlikely]]
		{
			new (&m_Modulatable) ::Modulatable();
			m_Modulatable = std::move(other.m_Modulatable);
		}
	}
	inline FickleSelector2D& operator=(const FickleSelector2D& other)
	{
		if (this == &other)
			return *this;
		DestroyUnion();
		type = other.type;
		if (type == FickleType::Protean) [[likely]]
			new (&m_Protean) Protean2D(other.m_Protean);
		else if (type == FickleType::Transformable)
			new (&m_Transformable) Transformable2D(other.m_Transformable);
		else if (type == FickleType::Modulatable) [[unlikely]]
			new (&m_Modulatable) ::Modulatable(other.m_Modulatable);
		return *this;
	}
	inline FickleSelector2D& operator=(FickleSelector2D&& other) noexcept
	{
		if (this == &other)
			return *this;
		DestroyUnion();
		type = other.type;
		if (type == FickleType::Protean) [[likely]]
			new (&m_Protean) Protean2D(std::move(other.m_Protean));
		else if (type == FickleType::Transformable)
			new (&m_Transformable) Transformable2D(std::move(other.m_Transformable));
		else if (type == FickleType::Modulatable) [[unlikely]]
			new (&m_Modulatable) ::Modulatable(std::move(other.m_Modulatable));
		return *this;
	}
	inline ~FickleSelector2D()
	{
		DestroyUnion();
	}
	inline FickleType Type() const
	{
		return type;
	}
	inline bool IsSubtype(FickleType supertype) const
	{
		if (supertype == FickleType::Protean) [[likely]]
			return true;
		else if (supertype == FickleType::Transformable)
			return type == FickleType::Transformable;
		else if (supertype == FickleType::Modulatable) [[unlikely]]
			return type == FickleType::Modulatable;
		return false;
	}
	inline bool IsProtean() const
	{
		return type == FickleType::Protean;
	}
	inline bool IsTransformable() const
	{
		return type == FickleType::Transformable;
	}
	inline bool IsModulatable() const
	{
		return type == FickleType::Modulatable;
	}
	inline bool CanTransform() const
	{
		return type == FickleType::Protean || type == FickleType::Transformable;
	}
	inline bool CanModulate() const
	{
		return type == FickleType::Protean || type == FickleType::Modulatable;
	}
	inline FickleNotification** Notification()
	{
		if (type == FickleType::Protean) [[likely]]
			return &m_Protean.Ref()->notify;
		else if (type == FickleType::Transformable)
			return &m_Transformable.Ref()->notify;
		else if (type == FickleType::Modulatable) [[unlikely]]
			return &m_Modulatable.Ref()->notify;
		return nullptr;
	}
	inline Protean2D* Protean()
	{
		if (IsProtean())
			return &m_Protean;
		return nullptr;
	}
	inline Transformable2D* Transformable()
	{
		if (IsTransformable())
			return &m_Transformable;
		return nullptr;
	}
	inline Modulatable* Modulatable()
	{
		if (IsModulatable())
			return &m_Modulatable;
		return nullptr;
	}
	inline Transformer2D* Transformer()
	{
		if (IsTransformable())
			return m_Transformable.Ref();
		return nullptr;
	}
	inline Modulator* Modulator()
	{
		if (IsModulatable())
			return m_Modulatable.Ref();
		return nullptr;
	}
	inline ProteanLinker2D* ProteanLinker()
	{
		if (IsProtean())
			return m_Protean.Ref();
		return nullptr;
	}
	inline Transform2D* Transform()
	{
		if (IsProtean())
			return &m_Protean.Ref()->Transform();
		if (IsTransformable())
			return &m_Transformable.Ref()->Self();
		return nullptr;
	}
	inline Position2D* Position()
	{
		if (IsProtean())
			return &m_Protean.Ref()->Position();
		if (IsTransformable())
			return &m_Transformable.Ref()->Position();
		return nullptr;
	}
	inline Rotation2D* Rotation()
	{
		if (IsProtean())
			return &m_Protean.Ref()->Rotation();
		if (IsTransformable())
			return &m_Transformable.Ref()->Rotation();
		return nullptr;
	}
	inline Scale2D* Scale()
	{
		if (IsProtean())
			return &m_Protean.Ref()->Scale();
		if (IsTransformable())
			return &m_Transformable.Ref()->Scale();
		return nullptr;
	}
	inline Modulate* Modulate()
	{
		if (IsProtean())
			return &m_Protean.Ref()->Modulate();
		if (IsModulatable())
			return &m_Modulatable.Ref()->Self();
		return nullptr;
	}
	inline PackedP2D* PackedP()
	{
		if (IsProtean())
			return &m_Protean.Ref()->self.packedP;
		if (IsTransformable())
			return &m_Transformable.Ref()->self.packedP;
		return nullptr;
	}
	inline PackedRS2D* PackedRS()
	{
		if (IsProtean())
			return &m_Protean.Ref()->self.packedRS;
		if (IsTransformable())
			return &m_Transformable.Ref()->self.packedRS;
		return nullptr;
	}
	inline ::Modulate* PackedM()
	{
		if (IsProtean())
			return &m_Protean.Ref()->self.packedM;
		if (IsModulatable())
			return &m_Modulatable.Ref()->self.packedM;
		return nullptr;
	}
	inline void Sync()
	{
		switch (type)
		{
		[[likely]] case FickleType::Protean:
			m_Protean.Ref()->Sync();
			break;
		case FickleType::Transformable:
			m_Transformable.Ref()->Sync();
			break;
		[[unlikely]] case FickleType::Modulatable:
			m_Modulatable.Ref()->Sync();
			break;
		}
	}
	inline void SyncT()
	{
		if (IsProtean()) [[likely]]
			m_Protean.Ref()->SyncT();
		else if (IsTransformable()) [[unlikely]]
			m_Transformable.Ref()->Sync();
	}
	inline void SyncP()
	{
		if (IsProtean()) [[likely]]
			m_Protean.Ref()->SyncP();
		else if (IsTransformable()) [[unlikely]]
			m_Transformable.Ref()->SyncP();
	}
	inline void SyncRS()
	{
		if (IsProtean()) [[likely]]
			m_Protean.Ref()->SyncRS();
		else if (IsTransformable()) [[unlikely]]
			m_Transformable.Ref()->SyncRS();
	}
	inline void SyncM()
	{
		if (IsProtean()) [[likely]]
			m_Protean.Ref()->SyncM();
		else if (IsModulatable()) [[unlikely]]
			m_Modulatable.Ref()->Sync();
	}

private:
	void DestroyUnion()
	{
		if (type == FickleType::Protean) [[likely]]
			m_Protean.~Protean2D();
		else if (type == FickleType::Transformable)
			m_Transformable.~Transformable2D();
		else if (type == FickleType::Modulatable) [[unlikely]]
			m_Modulatable.~Modulatable();
	}
};

class FickleUniqueVector
{
	union {
		std::vector<std::unique_ptr<ProteanLinker2D>> m_Linkers;
		std::vector<std::unique_ptr<Transformer2D>> m_Transformers;
		std::vector<std::unique_ptr<Modulator>> m_Modulators;
	};
	FickleType type;

public:
#pragma warning(suppress: 26495)
	inline FickleUniqueVector(FickleType type)
		: type(type)
	{
		switch (type)
		{
		case FickleType::Protean:
			new (&m_Linkers) std::vector<std::unique_ptr<ProteanLinker2D>>();
			break;
		[[likely]] case FickleType::Transformable:
			new (&m_Transformers) std::vector<std::unique_ptr<Transformer2D>>();
			break;
		[[unlikely]] case FickleType::Modulatable:
			new (&m_Modulators) std::vector<std::unique_ptr<Modulator>>();
			break;
		default:
			__debugbreak();
		}
	}
	FickleUniqueVector(const FickleUniqueVector&) = delete;
#pragma warning(suppress: 26495)
	inline FickleUniqueVector(FickleUniqueVector&& other) noexcept
		: type(other.type)
	{
		switch (type)
		{
		case FickleType::Protean:
			new (&m_Linkers) std::vector<std::unique_ptr<ProteanLinker2D>>(std::move(other.m_Linkers));
			break;
		[[likely]] case FickleType::Transformable:
			new (&m_Transformers) std::vector<std::unique_ptr<Transformer2D>>(std::move(other.m_Transformers));
			break;
		[[unlikely]] case FickleType::Modulatable:
			new (&m_Modulators) std::vector<std::unique_ptr<Modulator>>(std::move(other.m_Modulators));
			break;
		default:
			__debugbreak();
		}
	}
	FickleUniqueVector& operator=(const FickleUniqueVector&) = delete;
	inline FickleUniqueVector& operator=(FickleUniqueVector&& other) noexcept
	{
		if (this == &other)
			return *this;
		DestroyUnion();
		type = other.type;
		switch (type)
		{
		case FickleType::Protean:
			new (&m_Linkers) std::vector<std::unique_ptr<ProteanLinker2D>>(std::move(other.m_Linkers));
			break;
		[[likely]] case FickleType::Transformable:
			new (&m_Transformers) std::vector<std::unique_ptr<Transformer2D>>(std::move(other.m_Transformers));
			break;
		[[unlikely]] case FickleType::Modulatable:
			new (&m_Modulators) std::vector<std::unique_ptr<Modulator>>(std::move(other.m_Modulators));
			break;
		default:
			__debugbreak();
		}
		return *this;
	}
	inline ~FickleUniqueVector()
	{
		DestroyUnion();
	}
	inline FickleType Type() const
	{
		return type;
	}
	inline std::vector<std::unique_ptr<ProteanLinker2D>>* ProteanLinkers()
	{
		if (type == FickleType::Protean)
			return &m_Linkers;
		return nullptr;
	}
	inline std::vector<std::unique_ptr<Transformer2D>>* Transformers()
	{
		if (type == FickleType::Transformable)
			return &m_Transformers;
		return nullptr;
	}
	inline std::vector<std::unique_ptr<Modulator>>* Modulators()
	{
		if (type == FickleType::Modulatable)
			return &m_Modulators;
		return nullptr;
	}

private:
	void DestroyUnion()
	{
		if (type == FickleType::Protean)
			m_Linkers.~vector<std::unique_ptr<ProteanLinker2D>>();
		else if (type == FickleType::Transformable) [[likely]]
			m_Transformers.~vector<std::unique_ptr<Transformer2D>>();
		else if (type == FickleType::Modulatable) [[unlikely]]
			m_Modulators.~vector<std::unique_ptr<Modulator>>();
	}
};
