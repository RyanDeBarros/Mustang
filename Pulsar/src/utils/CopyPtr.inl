#pragma once

#include <type_traits>

template<typename T>
class CopyPtr
{
	T* ptr = nullptr;

public:
	template<typename U>
	friend class CopyPtr;

	CopyPtr(T* ptr = nullptr) : ptr(ptr) {}
	CopyPtr(const T& value) : ptr(new T(value)) {}
	CopyPtr(T&& value) : ptr(new T(std::move(value))) {}

	CopyPtr(const CopyPtr<T>& other) : ptr(other ? new T(*other.ptr) : nullptr) {}
	CopyPtr(CopyPtr<T>&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	CopyPtr<T>& operator=(const CopyPtr<T>& other)
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
	CopyPtr<T>& operator=(CopyPtr<T>&& other) noexcept
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
	CopyPtr(const CopyPtr<SubT>& other) : ptr(other ? new SubT(*other.ptr) : nullptr) {}
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	CopyPtr(CopyPtr<SubT>&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
	template<typename SubT, typename = std::enable_if_t<std::is_base_of_v<T, SubT> && !std::is_same_v<T, SubT>>>
	CopyPtr<T>& operator=(const CopyPtr<SubT>& other)
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
	CopyPtr<T>& operator=(CopyPtr<SubT>&& other) noexcept
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

	~CopyPtr() { if (ptr) delete ptr; }

	T* get() const { return ptr; }
	operator bool() const { return ptr; }
	T& operator*() const { return *ptr; }
	T* operator->() const { return ptr; }
};
