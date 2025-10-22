#pragma once
#include "binder.h"
#include <stdexcept>

class bnd3_t : public binder_t
{
	friend binder_t;
	std::string version;
	i32 format = 0;
	i32 unk18 = 0;
	bool big_endian = false;
	bool bit_big_endian = false;
	
	public:
	static bnd3_t* read(UMEM* mem)
	{
		struct Header
		{
			char magic[4];
			char version[8];
			uint8_t rawFormat;
			uint8_t bigEndian;
			uint8_t bitBigEndian;
			char unk0F;
			int32_t fileCount;
			int32_t fileHeadersEnd;
			int32_t unk18;
			int32_t unk1C; //assert 0
		};

		/*{
			Header h;
			mem->read(&h,sizeof(Header),1);
			std::string magic_str = "";
			for(i32 i = 0; i < 4; i++)
				magic_str.push_back(h.magic[i]);
			printf("magic: %s\n",magic_str.c_str());
			std::string ver_str = "";
			for(i32 i = 0; i < 8; i++)
				ver_str.push_back(h.version[i]);
			printf("version: %s\n",ver_str.c_str());
			printf("rawFormat: %u\n",h.rawFormat);
			printf("bigEndian: %u\n",h.bigEndian);
			printf("bitBigEndian: %u\n",h.bitBigEndian);
			printf("unk0F: %u\n",h.unk0F);
			printf("file_count: %i\n",h.fileCount);
			printf("unk18: %i\n",h.unk18);
			printf("unk1C: %i\n",h.unk1C);
			printf("--------------\n");
			mem->seek(-sizeof(Header),SEEK_CUR);
		}*/

		bnd3_t* bnd = new bnd3_t();

		auto assert_fn = [&](bool b, std::string err = "")
		{
			if(b == false)
			{
				delete bnd;
				printf("BND3 Assert Error: %s\n",err.c_str());
			}
			return b;
		};

		{
			char magic[4];
			mem->read(magic,sizeof(char),4);
			std::string magic_str = "";
			for(i32 i = 0; i < 4; i++)
				magic_str.push_back(magic[i]);
			//printf("magic: %s\n",magic_str.c_str());
			if(!assert_fn(same_str(magic,"BND3"),"got magic "+magic_str))
			{
				throw std::runtime_error("Ah\n");
				return nullptr;
			}
		}

		bnd->version = "00000000";
		mem->read(bnd->version.data(),sizeof(char),8);
		//printf("version: %s\n",bnd->version.c_str());

		mem->seek(2,SEEK_CUR);
		bnd->bit_big_endian = mem->read_u8();
		mem->seek(-3,SEEK_CUR);

		bnd->format = mem->read_format(bnd->bit_big_endian);

		bnd->big_endian = mem->read_u8();
		mem->assert_u8(bnd->bit_big_endian);
		mem->assert_u8(0);
		mem->big_endian() = bnd->big_endian || (bnd->format & format_e::fmt_big_endian);

		i32 file_count = mem->read_i32();
		i32 file_header_end = mem->read_i32();
		bnd->unk18 = mem->read_i32(); //TODO: assert a value for this
		i32 unk1C = mem->read_i32(); //TODO: assert a value for this

		//printf("file_count: %i\n",file_count);

		for(i32 i = 0; i < file_count; i++)
			bnd->file_headers.push_back(file_header_t::read_bnd3_fh(
				bnd,mem,bnd->format,bnd->bit_big_endian
			));

		return bnd;
	};

	static void write_header(UMEM* mem, bnd3_t* bnd)
	{
		mem->big_endian() = bnd->big_endian || (bnd->format & format_e::fmt_big_endian);

		char magic[] = "BND3";
		mem->write(magic,sizeof(char),4);
		mem->write(bnd->version.data(),sizeof(char),8);

		mem->write_format(bnd->format,bnd->big_endian);
		mem->write_u8(bnd->big_endian);
		mem->write_u8(bnd->bit_big_endian);
		mem->write_u8(0);

		mem->write_i32(bnd->file_headers.size());
		mem->reserve_i32("file_headers_end");
		mem->write_i32(bnd->unk18);
		mem->write_i32(0);

		for(i32 i = 0; i < bnd->file_headers.size(); i++)
			bnd->file_headers[i]->write_bnd3_header(mem,bnd->format,bnd->bit_big_endian,i);

		for(i32 i = 0; i < bnd->file_headers.size(); i++)
			bnd->file_headers[i]->write_name(mem,bnd->format,false,i);

		mem->fill_i32("file_headers_end",mem->m_pos);
	};

	void write_header(UMEM* mem) override {write_header(mem,this);};

	void write_data(UMEM* mem) override
	{
		//for(i32 i = 0; i < file_headers.size(); i++)
			//file_headers[i]->write_bnd3_data(mem, u8 *data, i64 length)
	};
};
