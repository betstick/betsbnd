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
	UMEM* m_src = nullptr;
	u32 m_uncompressed_size = 0;
	char m_format[4];

	static void decompress(UMEM* dst, UMEM* src, i32 compression_type)
	{
		i32 ret = 0;
		std::string type = std::to_string(compression_type);

		switch(compression_type)
		{
			case(CMP_DFLT):
				src->seek(4,SEEK_CUR);
				ret = zlib_inf(src,dst);
				break;
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

	public:
	static dcx_t* open(UMEM* src)
	{
		dcx_t* dcx = new dcx_t();

		char magic[4];
		src->read(magic,sizeof(char),4);

		std::string magic_str = "";
		for(i32 i = 0; i < 4; i++)
			magic_str.push_back(magic[i]);
		//printf("magic: %s\n",magic_str.c_str());

		i32 unk04 = src->read_i32();

		src->seek(8,SEEK_CUR);

		i32 unk10 = src->read_i32();

		src->seek(8,SEEK_CUR);

		dcx->m_uncompressed_size = src->read_u32();
		//printf("uncompressed size: %u\n",dcx->m_uncompressed_size);

		src->seek(8,SEEK_CUR);

		src->read(dcx->m_format,sizeof(char),4);

		std::string fmt_str = "";
		for(i32 i = 0; i < 4; i++)
			fmt_str.push_back(dcx->m_format[i]);
		//printf("fmt: %s\n",fmt_str.c_str());

		src->seek(4,SEEK_CUR);

		i8 unk30 = src->read_i8();

		src->seek(23,SEEK_CUR);

		if(memcmp(dcx->m_format,"EDGE",4) == 0)
		{
			//TODO: this
		}

		dcx->m_src = src;
		return dcx;
	};

	void decompress(UMEM* dst)
	{
		decompress(dst,m_src,CMP_DFLT);
	};
};
