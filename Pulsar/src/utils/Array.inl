#pragma once

#include <stdexcept>
#include <memory.h>
#include <initializer_list>

/// Fixed-size array that stores elements on heap, unlike std::array. Its size is fixed, unlike std::vector.
template<typename T>
class Array
{
	size_t m_Size = 0;
	T* m_Array = nullptr;

public:
	explicit Array(size_t size = 0) : m_Size(size) { if (m_Size > 0) m_Array = new T[m_Size]; }
	explicit Array(size_t size, T&& initial_value) : m_Size(size) { if (m_Size > 0) m_Array = new T[m_Size](initial_value); }
	Array(std::initializer_list<T> l) : m_Size(l.size())
	{
		if (m_Size > 0)
			m_Array = new T[m_Size];
		std::copy(l.begin(), l.end(), m_Array);
	}
	Array(const Array<T>& other) : m_Size(other.m_Size) 
	{
		if (m_Size > 0)
		{
			m_Array = new T[m_Size];
			for (size_t i = 0; i < m_Size; i++)
				new (m_Array + i) T(other.m_Array[i]);
		}
	}
	Array(Array<T>&& other) noexcept : m_Size(other.m_Size), m_Array(other.m_Array) { other.m_Size = 0; other.m_Array = nullptr; }
	Array<T>& operator=(const Array<T>& other)
	{
		if (other.m_Size == m_Size)
		{
			if (other.m_Array)
			{
				for (size_t i = 0; i < m_Size; i++)
					m_Array[i] = other.m_Array[i];
			}
		}
		else
		{
			if (m_Array)
				delete[] m_Array;
			m_Size = other.m_Size;
			if (other.m_Array)
			{
				m_Array = new T[m_Size];
				for (size_t i = 0; i < m_Size; i++)
					new (m_Array + i) T(other.m_Array[i]);
			}
			else
				m_Array = nullptr;
		}
		return *this;
	}
	Array<T>& operator=(Array<T>&& other) noexcept
	{
		if (m_Array)
			delete[] m_Array;
		m_Size = other.m_Size;
		m_Array = other.m_Array;
		other.m_Size = 0;
		other.m_Array = nullptr;
		return *this;
	}
	~Array() { if (m_Array) delete[] m_Array; }
	operator bool() const { return m_Size > 0; }

	T& operator[](size_t index) { if (index < m_Size) return m_Array[index]; else throw std::out_of_range("Index out of range"); }
	const T& operator[](size_t index) const { if (index < m_Size) return m_Array[index]; else throw std::out_of_range("Index out of range"); }

	size_t Size() const { return m_Size; }
};
