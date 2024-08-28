#pragma once

#include <type_traits>

template<typename T>
class CopyPtr
{
	T* ptr = nullptr;

public:
	template<typename U>
	friend class CopyPtr;

	inline CopyPtr(T* ptr = nullptr) : ptr(ptr) {}
	inline CopyPtr(const T& value) : ptr(new T(value)) {}
	inline CopyPtr(T&& value) : ptr(new T(std::move(value))) {}

	inline CopyPtr(const CopyPtr<T>& other) : ptr(other ? new T(*other.ptr) : nullptr) {}
	inline CopyPtr(CopyPtr<T>&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	inline CopyPtr<T>& operator=(const CopyPtr<T>& other)
	{
		if (this == &other)
			return *this;
		if (other)
		{
			ptr->~T();
			new (ptr) T(other);
		}
		else
		{
			if (ptr)
			{
				delete ptr;
				ptr = nullptr;
			}
		}
		return *this;
	}
	inline CopyPtr<T>& operator=(CopyPtr<T>&& other) noexcept
	{
		if (this == &other)
			return *this;
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}
	
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	inline CopyPtr(const CopyPtr<SubT>& other) : ptr(other ? new SubT(*other.ptr) : nullptr) {}
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	inline CopyPtr(CopyPtr<SubT>&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	inline CopyPtr<T>& operator=(const CopyPtr<SubT>& other)
	{
		if (other)
		{
			ptr->~T();
			new (ptr) T(other);
		}
		else
		{
			if (ptr)
			{
				delete ptr;
				ptr = nullptr;
			}
		}
		return *this;
	}
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	inline CopyPtr<T>& operator=(CopyPtr<SubT>&& other) noexcept
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	inline ~CopyPtr() { if (ptr) delete ptr; }

	inline T* get() const { return ptr; }
	inline operator bool() const { return ptr; }
	inline T& operator*() const { return *ptr; }
	inline T* operator->() const { return ptr; }
};
