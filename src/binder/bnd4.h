#pragma once
#include "binder.h"

class bnd4_t : public binder_t
{
	friend binder_t;
	std::string version;
	i32 format = 0;
	i8 unk04 = 0;
	i8 unk05 = 0;
	bool big_endian = false;
	bool bit_big_endian = false;
	bool unicode = false;
	u8 extended = 0;
	
	static bnd4_t* read(UMEM* mem)
	{
		std::string magic = "\0\0\0\0";
		mem->read(magic.data(),sizeof(char),4);
		if(!same_str(magic,"BND4"))
			return nullptr;

		bnd4_t* bnd = new bnd4_t();

		auto assert_fn = [&](bool b, std::string err = "")
		{
			if(b == false)
			{
				delete bnd;
				printf("BND4 Assert Error: %s\n",err.c_str());
			}
			return b;
		};
		
		mem->read(&bnd->unk04,sizeof(i8),1);
		mem->read(&bnd->unk05,sizeof(i8),1);

		for(i32 i = 0; i < 3; i++)
			if(!assert_fn(mem->assert_i8(0),"padding 0-2"))
				return nullptr;
		
		mem->read(&bnd->big_endian,sizeof(bool),1);
		mem->read(&bnd->bit_big_endian,sizeof(bool),1);
		bnd->bit_big_endian = !bnd->bit_big_endian;

		if(!assert_fn(mem->assert_i8(0),"padding 3"))
			return nullptr;

		mem->big_endian() = bnd->big_endian;

		i32 file_count = mem->read_i32();

		i64 header_size = 0;
		if(!assert_fn(mem->assert_i64(0x40,&header_size),"header size"))
			return nullptr;

		bnd->version = "00000000";
		mem->read(bnd->version.data(),sizeof(char),8);
		
		i64 file_header_size = mem->read_i64();

		mem->seek(sizeof(i64),SEEK_CUR);

		mem->read(&bnd->unicode,sizeof(bool),1);
		bnd->format = mem->read_format(bnd->big_endian);
		bnd->extended = mem->read_i8();

		if(!assert_fn(mem->assert_any_u8({0,1,4,0x80},&bnd->extended),"extended"))
			return nullptr;

		if(!assert_fn(mem->assert_i8(0),"padding 4"))
			return nullptr;

		if(!assert_fn(mem->assert_i32(0),"padding 5"))
			return nullptr;

		if(bnd->extended == 4)
		{
			i64 hash_table_offset = mem->read_i64();
			mem->step_in(hash_table_offset);
			if(!assert_fn(binder_hash_table_t::assert(mem),"hash table"))
				return nullptr;
			mem->step_out();
		}
		else
		{
			if(!assert_fn(mem->assert_i64(0),"padding 6"))
				return nullptr;
		}

		if(!assert_fn(file_header_size != binder_t::get_bnd4_file_header_size(bnd->format),"header size"))
			return nullptr;
		
		for(i32 i = 0; i < file_count; i++)
			bnd->file_headers.push_back(file_header_t::read_bnd4_fh(
				bnd,mem,bnd->format,bnd->bit_big_endian,bnd->unicode
			));

		return bnd;
	};

	static void write_header(UMEM* mem, bnd4_t* bnd)
	{
		mem->big_endian() = bnd->big_endian;

		char magic[] = "BND4";
		mem->write(magic,sizeof(char),4);

		mem->write(&bnd->unk04,sizeof(bool),1);
		mem->write(&bnd->unk05,sizeof(bool),1);
		mem->write_u8(0);
		mem->write_u8(0);
		mem->write_u8(0);

		mem->write(&bnd->big_endian,sizeof(bool),1);
		mem->write_u8(!bnd->bit_big_endian);
		mem->write_u8(0);

		mem->write_i32(bnd->file_headers.size());
		mem->write_i64(0x40);
		mem->write(bnd->version.data(),sizeof(char),8);
		mem->write_i64(binder_t::get_bnd4_file_header_size(bnd->format));
		mem->reserve_i64("file_headers_end");

		mem->write_u8(bnd->unicode);
		mem->write_format(bnd->format,bnd->bit_big_endian);
		mem->write_u8(bnd->extended);
		mem->write_u8(0);

		mem->write_i32(0);
		mem->reserve_i64("hash_table_offset");

		for(i32 i = 0; i < bnd->file_headers.size(); i++)
			bnd->file_headers[i]->write_bnd4_header(mem,bnd->format,bnd->bit_big_endian,i);

		for(i32 i = 0; i < bnd->file_headers.size(); i++)
			bnd->file_headers[i]->write_name(mem,bnd->format,bnd->unicode,i);

		if(bnd->extended)
		{
			mem->pad(0x8);
			mem->fill_i64("hash_table_offset",mem->m_pos);
			binder_hash_table_t::write(mem,bnd->file_headers);
		}
		else
		{
			mem->fill_i64("hash_table_offset",0);
		}

		mem->fill_i64("file_headers_end",mem->m_pos);
	};

	void write_header(UMEM* mem) override {write_header(mem,this);};

	void write_data(UMEM* mem) override {};
};