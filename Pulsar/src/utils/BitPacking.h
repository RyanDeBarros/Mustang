#pragma once

#include <cstdint>

#ifndef PULSAR_REINTERPRET
#define PULSAR_REINTERPRET(type, var) *PULSAR_REINTERPRET_cast<type*>(&var)
#endif

class BitPack_u32_u32
{
	uint64_t x;

public:
	BitPack_u32_u32(uint32_t _1, uint32_t _2) : x(
		(static_cast<uint64_t>(_1) << 32) | _2
	) {}
	uint32_t _1() const
	{
		return x >> 32;
	}
	uint32_t _2() const {
		return x;
	}
	void i1(uint32_t _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(_1) << 32);
	}
	void i2(uint32_t _2) {
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
	BitPack_32_32(T1 _1, T2 _2) : x(
		(static_cast<uint64_t>(PULSAR_REINTERPRET(uint32_t, _1)) << 32)
		| PULSAR_REINTERPRET(uint32_t, _2)
	) {}
	T1 _1() const
	{
		uint32_t y = x >> 32;
		return PULSAR_REINTERPRET(T1, y);
	}
	T2 _2() const
	{
		uint32_t y = x;
		return PULSAR_REINTERPRET(T2, y);
	}
	void _1(T1 _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint32_t, _1)) << 32);
	}
	void _2(T2 _2)
	{
		x = (x & 0xFFFFFFFF00000000) | PULSAR_REINTERPRET(uint32_t, _2);
	}
};

template<_32Bit T1, _16Bit T2, _16Bit T3 = T2>
class BitPack_32_16_16
{
	uint64_t x;

public:
	BitPack_32_16_16(T1 _1, T2 _2, T3 _3) : x(
		(static_cast<uint64_t>(PULSAR_REINTERPRET(uint32_t, _1)) << 32)
		| (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 16)
		| PULSAR_REINTERPRET(uint16_t, _3)
	) {}
	T1 _1() const
	{
		uint32_t y = x >> 32;
		return PULSAR_REINTERPRET(T1, y);
	}
	T2 _2() const
	{
		uint16_t y = x >> 16;
		return PULSAR_REINTERPRET(T2, y);
	}
	T3 _3() const
	{
		uint16_t y = x;
		return PULSAR_REINTERPRET(T3, y);
	}
	void _1(T1 _1)
	{
		x = (x & 0x00000000FFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint32_t, _1)) << 32);
	}
	void _2(T2 _2)
	{
		x = (x & 0xFFFFFFFF0000FFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 16);
	}
	void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFFFFFF0000) | PULSAR_REINTERPRET(uint16_t, _3);
	}
};

template<_16Bit T1, _16Bit T2 = T1, _32Bit T3>
class BitPack_16_16_32
{
	uint64_t x;

public:
	BitPack_16_16_32(T1 _1, T2 _2, T3 _3) : x(
		(static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 48)
		| (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 32)
		| PULSAR_REINTERPRET(uint32_t, _3)
	) {}
	T1 _1() const
	{
		uint16_t y = x >> 48;
		return PULSAR_REINTERPRET(T1, y);
	}
	T2 _2() const
	{
		uint16_t y = x >> 32;
		return PULSAR_REINTERPRET(T2, y);
	}
	T3 _3() const
	{
		uint32_t y = x;
		return PULSAR_REINTERPRET(T3, y);
	}
	void _1(T1 _1)
	{
		x = (x & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 48);
	}
	void _2(T2 _2)
	{
		x = (x & 0xFFFF0000FFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 32);
	}
	void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFF00000000) | PULSAR_REINTERPRET(uint32_t, _3);
	}
};

template<_16Bit T1, _16Bit T2 = T1, _16Bit T3 = T1, _16Bit T4 = T1>
class BitPack_16_16_16_16
{
	uint64_t x;

public:
	BitPack_16_16_16_16(T1 _1, T2 _2, T3 _3, T4 _4) : x(
		(static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 48)
		| (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 32)
		| (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _3)) << 16)
		| PULSAR_REINTERPRET(uint16_t, _4)
	) {}
	T1 _1() const
	{
		uint16_t y = x >> 48;
		return PULSAR_REINTERPRET(T1, y);
	}
	T2 _2() const
	{
		uint16_t y = x >> 32;
		return PULSAR_REINTERPRET(T2, y);
	}
	T3 _3() const
	{
		uint16_t y = x >> 16;
		return PULSAR_REINTERPRET(T3, y);
	}
	T4 _4() const
	{
		uint16_t y = x;
		return PULSAR_REINTERPRET(T4, y);
	}
	void _1(T1 _1)
	{
		x = (x & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 48);
	}
	void _2(T2 _2)
	{
		x = (x & 0xFFFF0000FFFFFFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _2)) << 32);
	}
	void _3(T3 _3)
	{
		x = (x & 0xFFFFFFFF0000FFFF) | (static_cast<uint64_t>(PULSAR_REINTERPRET(uint16_t, _3)) << 16);
	}
	void _4(T4 _4)
	{
		x = (x & 0xFFFFFFFFFFFF0000) | PULSAR_REINTERPRET(uint16_t, _4);
	}
};

template<_16Bit T1, _16Bit T2 = T1>
class BitPack_16_16
{
	uint32_t x;

public:
	BitPack_16_16(T1 _1, T2 _2) : x(
		(static_cast<uint32_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 16)
		| PULSAR_REINTERPRET(uint16_t, _2)
	) {}
	T1 _1() const
	{
		uint16_t y = x >> 16;
		return PULSAR_REINTERPRET(T1, y);
	}
	T2 _2() const
	{
		uint16_t y = x;
		return PULSAR_REINTERPRET(T2, y);
	}
	void _1(T1 _1)
	{
		x = (x & 0x0000FFFF) | (static_cast<uint32_t>(PULSAR_REINTERPRET(uint16_t, _1)) << 16);
	}
	void _2(T2 _2)
	{
		x = (x & 0xFFFF0000) | PULSAR_REINTERPRET(uint16_t, _2);
	}
};
