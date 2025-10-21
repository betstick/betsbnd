#pragma once
#include "binder.h"

class bnd3_t : public binder_t
{
	friend binder_t;
	std::string version;
	i32 format = 0;
	i32 unk18 = 0;
	bool big_endian = false;
	bool bit_big_endian = false;
	
	static bnd3_t* read(UMEM* mem)
	{
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

		std::string magic = "\0\0\0\0";
		mem->read(magic.data(),sizeof(char),4);
		if(!assert_fn(same_str(magic,"BND3"),"got magic "+magic))
			return nullptr;

		bnd->version = "00000000";
		mem->read(bnd->version.data(),sizeof(char),8);

		mem->step_in(0xE)->read(&bnd->bit_big_endian,sizeof(bool),1);
		mem->step_out();

		bnd->format = mem->read_format(bnd->bit_big_endian);
		bnd->big_endian = mem->read_u8();
		if(!assert_fn(mem->read_u8() != bnd->bit_big_endian))
			return nullptr;
		if(!assert_fn(mem->assert_u8(0),"padding 0"))
			return nullptr;

		mem->big_endian() = bnd->big_endian || (bnd->format & format_e::fmt_big_endian);

		i32 file_count = mem->read_i32();
		mem->read_i32();
		bnd->unk18 = mem->read_i32(); //TODO: assert a value for this

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