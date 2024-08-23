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
	inline explicit Array(size_t size) : m_Size(size) { if (m_Size > 0) m_Array = new T[m_Size]; }
	inline Array(std::initializer_list<T> l) : m_Size(l.size())
	{
		if (m_Size > 0)
			m_Array = new T[m_Size];
		std::copy(l.begin(), l.end(), m_Array);
	}
	inline Array(const Array<T>& other) : m_Size(other.m_Size) 
	{
		if (m_Size > 0)
		{
			m_Array = new T[m_Size];
			memcpy_s(m_Array, m_Size, other.m_Array, other.m_Size);
		}
	}
	inline Array(Array<T>&& other) noexcept : m_Size(other.m_Size), m_Array(other.m_Array) { other.m_Size = 0; other.m_Array = nullptr; }
	inline Array<T>& operator=(const Array<T>& other)
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
				memcpy_s(m_Array, m_Size, other.m_Array, other.m_Size);
			}
			else
			{
				m_Array = nullptr;
			}
		}
		return *this;
	}
	inline Array<T>& operator=(Array<T>&& other) noexcept
	{
		if (m_Array)
			delete[] m_Array;
		m_Size = other.m_Size;
		m_Array = other.m_Array;
		other.m_Size = 0;
		other.m_Array = nullptr;
		return *this;
	}
	inline ~Array() { if (m_Array) delete[] m_Array; }

	inline T& operator[](size_t index) { if (index < m_Size) return m_Array[index]; else throw std::out_of_range("Index out of range"); }
	inline const T& operator[](size_t index) const { if (index < m_Size) return m_Array[index]; else throw std::out_of_range("Index out of range"); }

	inline size_t Size() const { return m_Size; }
};
