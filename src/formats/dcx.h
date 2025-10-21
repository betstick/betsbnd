#pragma once
#include "../common.h"
#include "../util/umem.h"
#include "../compression/zlib_inf.h"
#include <stdexcept>

enum compression_e
{
	CMP_DFLT,
	CMP_EDGE,
	CMP_KRAK,
	CMP_ZSTD
};

class dcx_t
{
	static void decompress(UMEM* dst, UMEM* src, i32 compression_type)
	{
		i32 ret = 0;
		std::string type = std::to_string(compression_type);

		switch(compression_type)
		{
			case(CMP_DFLT): ret = zlib_inf(src,dst); break;
			default:
				throw std::runtime_error("Unknown tpye: "+type+"\n");
		}

		std::string err = std::to_string(ret);
		if(ret != 0)
			throw std::runtime_error("Decompress failed! Type: "+type+", error: "+err+"\n");
	};

	static void compress(UMEM* dst, UMEM* src, i32 compression_type)
	{

	};
};