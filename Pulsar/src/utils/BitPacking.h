#pragma once

#include <cstdint>

// TODO use ifndef for all macros
#ifndef REINTERPRET
#define REINTERPRET(type, var) *reinterpret_cast<type*>(&var)
#endif

class BitPack_u32_u32
{
	uint64_t x;

public:
	inline BitPack_u32_u32(uint32_t _1, uint32_t _2) : x(
		(static_cast<uint64_t>(_1) << 32) | _2
	) {}
	inline uint32_t _1() const
	{
		return x >> 32;
	}
	inline uint32_t _2() const {
		return x;
	}
	inline void i1(uint32_t _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(_1) << 32);
	}
	inline void i2(uint32_t _2) {
		x = (x & 0xFFFFFFFF00000000) | _2;
	}
};

template<typename T>
concept _32Bit = sizeof(T) == 4;
template<typename T>
concept _24Bit = sizeof(T) == 3;
template<typename T>
concept _16Bit = sizeof(T) == 2;
template<typename T>
concept _8Bit = sizeof(T) == 1;

template<_32Bit T1, _32Bit T2 = T1>
class BitPack_32_32
{
	uint64_t x;

public:
	inline BitPack_32_32(T1 _1, T2 _2) : x(
		(static_cast<uint64_t>(REINTERPRET(uint32_t, _1)) << 32)
		| REINTERPRET(uint32_t, _2)
	) {}
	inline T1 _1() const
	{
		uint32_t y = x >> 32;
		return REINTERPRET(T1, y);
	}
	inline T2 _2() const
	{
		uint32_t y = x;
		return REINTERPRET(T2, y);
	}
	inline void _1(T1 _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint32_t, _1)) << 32);
	}
	inline void _2(T2 _2)
	{
		x = (x & 0xFFFFFFFF00000000) | REINTERPRET(uint32_t, _2);
	}
};

template<_32Bit T1, _16Bit T2, _16Bit T3 = T2>
class BitPack_32_16_16
{
	uint64_t x;

public:
	inline BitPack_32_16_16(T1 _1, T2 _2, T3 _3) : x(
		(static_cast<uint64_t>(REINTERPRET(uint32_t, _1)) << 32)
		| (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 16)
		| REINTERPRET(uint16_t, _3)
	) {}
	inline T1 _1() const
	{
		uint32_t y = x >> 32;
		return REINTERPRET(T1, y);
	}
	inline T2 _2() const
	{
		uint16_t y = x >> 16;
		return REINTERPRET(T2, y);
	}
	inline T3 _3() const
	{
		uint16_t y = x;
		return REINTERPRET(T3, y);
	}
	inline void _1(T1 _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint32_t, _1)) << 32);
	}
	inline void _2(T2 _2)
	{
		x = (x & 0xFFFFFFFF0000FFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 16);
	}
	inline void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFFFFFF0000) | REINTERPRET(uint16_t, _3);
	}
};

template<_16Bit T1, _16Bit T2 = T1, _32Bit T3>
class BitPack_16_16_32
{
	uint64_t x;

public:
	inline BitPack_16_16_32(T1 _1, T2 _2, T3 _3) : x(
		(static_cast<uint64_t>(REINTERPRET(uint16_t, _1)) << 48)
		| (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 32)
		| REINTERPRET(uint32_t, _3)
	) {}
	inline T1 _1() const
	{
		uint16_t y = x >> 48;
		return REINTERPRET(T1, y);
	}
	inline T2 _2() const
	{
		uint16_t y = x >> 32;
		return REINTERPRET(T2, y);
	}
	inline T3 _3() const
	{
		uint32_t y = x;
		return REINTERPRET(T3, y);
	}
	inline void _1(T1 _1)
	{
		x = (x & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _1)) << 48);
	}
	inline void _2(T2 _2)
	{
		x = (x & 0xFFFF0000FFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 32);
	}
	inline void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFF00000000) | REINTERPRET(uint32_t, _3);
	}
};

template<_16Bit T1, _16Bit T2 = T1, _16Bit T3 = T1, _16Bit T4 = T1>
class BitPack_16_16_16_16
{
	uint64_t x;

public:
	inline BitPack_16_16_16_16(T1 _1, T2 _2, T3 _3, T4 _4) : x(
		(static_cast<uint64_t>(REINTERPRET(uint16_t, _1)) << 48)
		| (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 32)
		| (static_cast<uint64_t>(REINTERPRET(uint16_t, _3)) << 16)
		| REINTERPRET(uint16_t, _4)
	) {}
	inline T1 _1() const
	{
		uint16_t y = x >> 48;
		return REINTERPRET(T1, y);
	}
	inline T2 _2() const
	{
		uint16_t y = x >> 32;
		return REINTERPRET(T2, y);
	}
	inline T3 _3() const
	{
		uint16_t y = x >> 16;
		return REINTERPRET(T3, y);
	}
	inline T4 _4() const
	{
		uint16_t y = x;
		return REINTERPRET(T4, y);
	}
	inline void _1(T1 _1)
	{
		x = (x & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _1)) << 48);
	}
	inline void _2(T2 _2)
	{
		x = (x & 0xFFFF0000FFFFFFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _2)) << 32);
	}
	inline void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFF0000FFFF) | (static_cast<uint64_t>(REINTERPRET(uint16_t, _3)) << 16);
	}
	inline void _4(T4 _4)
	{
		x = (x & 0xFFFFFFFFFFFF0000) | REINTERPRET(uint16_t, _4);
	}
};

template<_16Bit T1, _16Bit T2 = T1>
class BitPack_16_16
{
	uint32_t x;

public:
	inline BitPack_16_16(T1 _1, T2 _2) : x(
		(static_cast<uint32_t>(REINTERPRET(uint16_t, _1)) << 16)
		| REINTERPRET(uint16_t, _2)
	) {}
	inline T1 _1() const
	{
		uint16_t y = x >> 16;
		return REINTERPRET(T1, y);
	}
	inline T2 _2() const
	{
		uint16_t y = x;
		return REINTERPRET(T2, y);
	}
	inline void _1(T1 _1)
	{
		x = (x & 0x0000FFFF) | (static_cast<uint32_t>(REINTERPRET(uint16_t, _1)) << 16);
	}
	inline void _2(T2 _2)
	{
		x = (x & 0xFFFF0000) | REINTERPRET(uint16_t, _2);
	}
};
