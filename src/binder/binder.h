#pragma once
#include "../common.h"
#include "../util/strings.h"
#include "../util/umem.h"
#include "../util/util.h"

class binder_t;
class bnd3_t;
class bnd4_t;

enum format_e
{
	fmt_big_endian   = 0b00000001,
	fmt_ids          = 0b00000010,
	fmt_names1       = 0b00000100,
	fmt_names2       = 0b00001000,
	fmt_long_offsets = 0b00010000,
	fmt_compression  = 0b00100000,
	fmt_flag6        = 0b01000000,
	fmt_flag7        = 0b10000000,
};

enum file_flags_e
{
	ff_compressed = 0b00000001,
	ff_flag1      = 0b00000010,
	ff_flag2      = 0b00000100,
	ff_flag3      = 0b00001000,
	ff_flag4      = 0b00010000,
	ff_flag5      = 0b00100000,
	ff_flag6      = 0b01000000,
	ff_flag7      = 0b10000000,
};

class file_header_t
{
	friend bnd3_t;
	friend bnd4_t;
	binder_t* m_binder = nullptr; //parent binder
	std::string m_name;
	i32 m_file_flags = 0;
	i32 m_id = 0;
	i32 m_compression_type = 0; //TODO: figure out this
	i64 m_compressed_size = 0;
	i64 m_uncompressed_size = 0;
	i64 m_data_offset = 0;
	bool m_modified = false;
 
	static file_header_t* read_bnd3_fh(binder_t* bnd, UMEM* mem, i32 fmt, bool bbe)
	{
		file_header_t* fh = new file_header_t();
		fh->m_binder = bnd;

		fh->m_file_flags = mem->read_file_flags(bbe);
		for(i32 i = 0; i < 3; i++)
			mem->assert_u8(0);

		fh->m_compressed_size = mem->read_i32();
		fh->m_data_offset = 0;
		if(fmt & format_e::fmt_long_offsets)
			fh->m_data_offset = mem->read_i64();
		else
			fh->m_data_offset = mem->read_u32();

		fh->m_id = -1;
		if(fmt & format_e::fmt_ids)
			fh->m_id = mem->read_i32();

		if(fmt & (format_e::fmt_names1 |format_e::fmt_names1))
		{
			i32 name_offset = mem->read_i32();
			fh->m_name = mem->read_shift_jis(name_offset);
		}

		fh->m_uncompressed_size = -1;
		if(fmt & (format_e::fmt_compression))
			fh->m_uncompressed_size = mem->read_i32();

		return fh;
	};

	void write_bnd3_header(UMEM* mem, i32 format, bool bit_big_endian, i32 i)
	{
		mem->write_file_flags(m_file_flags,bit_big_endian);
		for(i32 i = 0; i < 3; i++)
			mem->write_u8(0);

		std::string index = std::to_string(i);

		mem->reserve_i32("compressed_size"+index);

		if(format & format_e::fmt_long_offsets)
			mem->reserve_i64("data_offset"+index);
		else
			mem->reserve_u32("data_offset"+index);

		if(format & format_e::fmt_ids)
			mem->write_i32(m_id);
	
		if(format & (format_e::fmt_names1 | format_e::fmt_names2))
			mem->reserve_i32("name_offset"+index);

		if(format & format_e::fmt_compression)
			mem->reserve_i32("uncompressed_size"+index);
	};

	static file_header_t* read_bnd4_fh(binder_t* bnd, UMEM* mem, i32 fmt, bool bbe, bool unicode)
	{
		file_header_t* fh = new file_header_t();
		fh->m_binder = bnd;
		
		fh->m_file_flags = mem->read_file_flags(bbe);

		auto assert_fn = [&](bool b, std::string err = "")
		{
			if(b == false)
			{
				delete fh;
				printf("File Header Assert Error: %s\n",err.c_str());
			}
			return b;
		};
		
		for(i32 i = 0; i < 3; i++)
			if(!assert_fn(mem->assert_i8(0),"padding 0-2"))
				return nullptr;
		
		if(!assert_fn(mem->assert_i32(-1),"padding 3"))
			return nullptr;

		fh->m_compressed_size = mem->read_i64();

		fh->m_uncompressed_size = -1;
		if(fmt & format_e::fmt_compression)
			fh->m_uncompressed_size = mem->read_i64();

		if(fmt & format_e::fmt_long_offsets)
			fh->m_data_offset = mem->read_i64();
		else
			fh->m_data_offset = mem->read_u32();
		
		fh->m_id = -1;
		if(fmt & format_e::fmt_ids)
			fh->m_id = mem->read_i32();

		if(fmt & (format_e::fmt_names1 | format_e::fmt_names2))
		{
			u32 name_offset = mem->read_u32();
			if(unicode)
				fh->m_name = mem->read_utf16(name_offset);
			else
				fh->m_name = mem->read_shift_jis(name_offset);
		}

		if(fmt == format_e::fmt_names1)
		{
			fh->m_id = mem->read_i32();
			if(!assert_fn(mem->assert_i32(0),"padding 4"))
				return nullptr;
		}

		return fh;
	};

	void write_bnd4_header(UMEM* mem, i32 format, bool bit_big_endian, i32 i)
	{
		mem->write_file_flags(m_file_flags,bit_big_endian);
		for(i32 i = 0; i < 3; i++)
			mem->write_u8(0);
		mem->write_i32(-1);

		std::string index = std::to_string(i);

		mem->reserve_i64("compressed_size"+index);

		if(format & format_e::fmt_compression)
			mem->reserve_i64("uncompressed_size"+index);

		if(format & format_e::fmt_long_offsets)
			mem->reserve_i64("data_offset"+index);
		else
			mem->reserve_u32("data_offset"+index);

		if(format & format_e::fmt_ids)
			mem->write_i32(m_id);

		if(format & (format_e::fmt_names1 | format_e::fmt_names2))
			mem->reserve_i32("name_offset"+index);

		if(format == format_e::fmt_names1)
		{
			mem->write_i32(m_id);
			mem->write_i32(0);
		}
	};

	void read_bnd3_data()
	{

	};

	void read_bnd4_data()
	{

	};

	void write_bnd3_data(UMEM* mem, u8* data, i64 length)
	{
		if(length > 0)
			mem->pad(0x10);

		m_data_offset = mem->m_pos;
		m_uncompressed_size = length;
		if(m_file_flags & file_flags_e::ff_compressed)
		{
			//TODO: write using compression type
		}
		else
		{
			m_compressed_size = length;
			mem->write(data,1,length);
		}
	};

	void write_bnd4_data(UMEM* mem, u8* data, i64 length)
	{
		if(length > 0)
			mem->pad(0x10);

		m_data_offset = mem->m_pos;
		m_uncompressed_size = length;
		if(m_file_flags & file_flags_e::ff_compressed)
		{
			//TODO: write using compression type
		}
		else
		{
			m_compressed_size = length;
			mem->write(data,1,length);
		}
	};

	void write_name(UMEM* mem, i32 format, bool unicode, i32 i)
	{
		if(format & (format_e::fmt_names1 | format_e::fmt_names2))
		{
			std::string index = std::to_string(i);
			mem->fill_i32("name_offset"+index,mem->m_pos);
			if(unicode)
				mem->write_utf16(m_name,true);
			else
				mem->write_shift_jis(m_name,true);
		}
	};

	public:
	std::string name() const {return m_name;};

	i64 size() const {return std::max<i64>(m_compressed_size,m_uncompressed_size);};
	
	i64 compressed_size() const {return m_compressed_size;};

	i64 offset() const {return m_data_offset;};

	i32 id() const {return m_id;};
	
	//read file data from binder and copy it to dst
	i32 read(UMEM* dst);

	//write file data to binder from src
	i32 write(UMEM* src);
};

class binder_t
{
	protected:
	UMEM* m_mem = nullptr;
	std::vector<file_header_t*> file_headers = {};

	virtual void write_header(UMEM* mem) = 0;

	virtual void write_data(UMEM* mem) = 0;

	static i64 get_bnd4_file_header_size(i32 format)
	{
		return 0x10
			+ (format & format_e::fmt_long_offsets ? 8 : 4)
			+ (format & format_e::fmt_compression  ? 8 : 0)
			+ (format & format_e::fmt_ids ? 4 : 0)
			+ (format & format_e::fmt_names1 | format_e::fmt_names2 ? 4 : 0)
			+ (format == format_e::fmt_names1 ? 8 : 0);
	};

	public:
	static binder_t* read(UMEM* mem);

	void write();

	UMEM* data() const {return m_mem;};

	void add_file(file_header_t* file);
	
	void remove_file(file_header_t* file);	
};

class binder_hash_table_t
{
	public:
	static bool assert(UMEM* mem)
	{
		u64 temp;
		mem->read(&temp,sizeof(i64),1);
		mem->read(&temp,sizeof(i32),1);
		if(!mem->assert_u8(0x10))
			return false;
		for(i32 i = 0; i < 2; i++)
			if(!mem->assert_u8(8))
				return false;
		if(!mem->assert_u8(0))
			return false;
		return true;
	};

	static void write(UMEM* mem, std::vector<file_header_t*> headers)
	{

	};
};