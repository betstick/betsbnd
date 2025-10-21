#pragma once
//#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <bit>
#include <algorithm>
//#pragma warning (disable: 4244)

inline void _BitScanForward(uint32_t* bitindex, int64_t mask)
{
	for(int32_t i = 0; i < 128; i++)
	{
		if(mask & 0x1)
		{
			*bitindex = i;
			break;
		}
		mask >>= 1;
	}
};

inline void _BitScanReverse(uint32_t* bitindex, int64_t mask)
{
	for(int32_t i = 0; i < 128; i++)
	{
		if(mask & int64_t(1) << 63)
		{
			*bitindex = i;
			break;
		}
		mask <<= 1;
	}
};

inline uint16_t _byteswap_ushort(uint16_t val)
{
	union
	{
		uint16_t x;
		uint8_t u[2];
	} a;
	a.x = val;
	std::swap(a.u[0],a.u[1]);
	return val;
};

inline uint32_t __byteswap_ulong(uint32_t val)
{
	union
	{
		uint32_t x;
		uint8_t u[4];
	} a;
	a.x = val;
	std::swap(a.u[0],a.u[3]);
	std::swap(a.u[1],a.u[2]);
	return val;
};

inline uint64_t _byteswap_uint64(uint64_t val)
{
	union
	{
		uint64_t x;
		uint8_t u[8];
	} a;
	a.x = val;
	std::swap(a.u[0],a.u[7]);
	std::swap(a.u[1],a.u[6]);
	std::swap(a.u[2],a.u[5]);
	std::swap(a.u[3],a.u[4]);
	return val;
};