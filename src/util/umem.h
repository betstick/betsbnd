#pragma once
#include "../common.h"
#include "../math/vec.h"
#include "types.h"
#include "strings.h"
#include "util.h"
#include <stdexcept>
#include <string>
#include <stack>

inline i64 block_alloc(i64 bytes, i64 block_size)
{
	return ((bytes + block_size + 1) / block_size) * block_size;
};

/*
	universal memory accessor
	uses a single interface to interact with files and memory
	also includes extra functionality from binaryreaderex
*/

struct UMEM
{
	enum status_e
	{
		ERR_FILE_FAILED_TO_OPEN,
		SUCCESS,
	};

	enum error_e
	{
		ERR_EOF,
		ERR_FAILED_TO_OPEN,
		ERR_NO_WRITE_PERM,
	};

	UMEM* m_parent = nullptr;
	FILE* m_file = nullptr;
	u8*   m_data = nullptr; //size of m_cap
	umap<std::string,i64> m_reservations = {};
	std::string m_path;
	std::string m_mode;
	std::stack<i64> m_stack = {};
	i64 m_bs = 1024; //block size for data allocation
	i64 m_pos = 0;  //position, less than or equal to m_cap
	i64 m_cap = 0;  //capacity (0 if file), set in increments of m_bs 
	i64 m_size = 0; //user facing size, less than or equal to m_cap
	i32 m_err = 0;
	//i32 m_code = 0;
	bool m_is_file = false;
	bool m_can_read = false;
	bool m_can_write = false;
	bool m_big_endian = false;

	UMEM(std::string p, std::string m)
	{
		m_path = p;
		m_mode = m;
		m_file = fopen(m_path.c_str(),m_mode.c_str());
		m_is_file = true;
		if(m_file == nullptr)
		{
			//m_code = ERR_FILE_FAILED_TO_OPEN;
			m_can_read = false;
			m_can_write = false;
		}
		else
		{
			//m_code = SUCCESS;
			m_can_read = true;
			if(m_mode.find('w') != -1)
				m_can_write = true;
			fseek(m_file,0,SEEK_END);
			m_size = ftell(m_file);
			fseek(m_file,0,SEEK_SET);
		}
	};

	UMEM(i64 new_size)
	{
		m_size = new_size;
		m_cap = block_alloc(new_size,m_bs);
		m_data = new u8[m_cap];
		memset(m_data,0,m_cap);
		m_can_read = true;
		m_can_write = true;
	};

	UMEM(UMEM* mem, i64 offset = 0)
	{
		m_parent = mem;
		m_file = nullptr;
		m_data = mem->m_data;

		m_bs = 0;
		m_cap = 0;
		m_pos = mem->m_pos + offset;
		m_size = mem->m_size;

		m_is_file = false;
		m_can_read = true;
		m_can_write = false;
	};

	~UMEM()
	{
		if(m_is_file)
		{
			if(m_file != nullptr)
				fclose(m_file);
			m_file = nullptr;
		}
		else if(m_parent == nullptr)
		{
			if(m_data != nullptr)
				delete[] m_data;
			m_data = nullptr;
		}
	};

	UMEM* step_in(i64 offset)
	{
		if(m_pos + offset < 0 || m_pos + offset >= m_size)
			m_err = EOF;
		m_stack.push(m_pos);
		m_pos = m_pos + offset;
		return this;
	};

	UMEM* step_out()
	{
		m_pos = m_stack.top();
		m_stack.pop();
		return this;
	};

	//reads out raw bytes from the source and copies to dst
	i64 read(void* dst, i64 size, i64 n)
	{
		i64 r = 0;
		if(m_is_file && m_can_read)
		{
			r = fread(dst,size,n,m_file);
			m_pos += r * size;
		}
		else
		{
			for(i64 i = 0; i < n; i++)
			{
				if(size + m_pos >= m_size)
				{
					m_err = EOF;
					break;
				}
				
				memcpy((char*)dst+(i*size),m_data+m_pos,size);
				m_pos += size;
				r += 1;
			}
		}
		return r;
	};

	i64 write(void* src, i64 size, i64 n)
	{
		if(!m_can_write)
			return 0;

		i64 r = 0;
		if(m_is_file)
		{
			r = fwrite(src,size,n,m_file);
			m_pos += r * size;
			if(m_pos > m_size)
				m_size = m_pos;
		}
		else
		{
			if(m_size + size * n > m_size)
				resize(m_size + size * n);
			memcpy(m_data+m_pos,src,size*n);
			r = n;
			m_pos += r * size;
		}
		return r;
	};

	void resize(i64 new_size)
	{
		if(new_size <= 0)
			throw std::runtime_error(
				"UMEM::resize() tried to allocate: "+std::to_string(new_size)+" bytes!\n"
			);

		if(m_is_file || !m_can_write)
			return;

		i64 old_size = m_size;
		i64 old_cap  = m_cap;

		if(new_size <= m_cap)
		{
			m_size = new_size;
			return;
		}

		//use this to prevent small reallocations
		m_cap  = block_alloc(new_size,m_bs);
		m_size = new_size;
		u8* old_ptr = m_data;
		u8* m_data = new u8[m_cap];
		memset(m_data,0,m_cap);
		memcpy(m_data,old_ptr,old_size);;
		delete old_ptr;
	};

	i32 seek(i64 offset, i32 whence)
	{
		if(m_is_file)
		{
#ifdef WIN32
			return _fseeki64(m_file,m_offset,whence);
#else
			return fseek(m_file,offset,whence);
#endif
		}
		else
		{
			switch(whence)
			{
				default:
				case(SEEK_SET):
					if(offset < 0 || offset >= m_size)
						return -1;
					m_pos = offset; return 0;
				case(SEEK_CUR):
					if(offset + m_pos < 0 || offset + m_pos >= m_size)
						return -1;
					m_pos += offset;
					return 0;
				case(SEEK_END):
					if(offset + m_size < 0 || offset + m_size >= m_size)
						return -1;
					m_pos = m_size + offset;
					return 0;
			}
		}
	};

	i64 tell() const {return m_pos;};

	bool is_file() const {return m_is_file;};

	bool can_read() const {return m_can_read;};

	bool can_write() const {return m_can_write;};

	bool in_bounds(i64 offset) const
	{
		return ((m_pos + offset) >= 0) && ((m_pos + offset) < m_size);
	};

	/*template <typename T>
	bool assert(T x, T* dst = nullptr)
	{
		T d;
		read(&d,sizeof(T),1);
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};*/

	/*template <typename T>
	bool assert_any(std::vector<T> v, T* dst = nullptr)
	{
		T d;
		read(&d,sizeof(T),1);
		if(dst != nullptr)
			*dst = d;
		for(i32 i = 0; i < v.size(); i++)
			if(d == v[i])
				return true;
		std::string expected = {};
		for(i32 i = 0; i < v.size(); i++)
			expected += (std::to_string(v[i])+",");
		printf(
			"UMEM assert: Got %s, expected {%s}\n",
			std::to_string(d).c_str(),expected.c_str()
		);
		return false;
	};*/

	std::string read_str(i64 offset)
	{
		step_in(offset);
		std::string str = "";
		while(true)
		{
			char c = read_u8();
			str.push_back(c);
			if(c == '\0')
				break;
		}
		step_out();
		return str;
	};

	void write_str(std::string str, bool terminate)
	{
		for(i32 i = 0; i < str.length(); i++)
			write_u8(str[i]);
		if(terminate)
			write_u8('\0');
	};

	std::string read_utf16(i64 offset)
	{
		return str_conv::utf16_to_utf8(read_str(offset),m_big_endian);
	};

	std::string read_shift_jis(i64 offset)
	{
		return str_conv::shift_jis_to_utf8(read_str(offset));
	};

	void write_utf16(std::string str, bool terminate)
	{
		write_str(str_conv::utf8_to_utf16(str,m_big_endian),terminate);
	};

	void write_shift_jis(std::string str, bool terminate)
	{
		write_str(str_conv::utf8_to_shift_jis(str),terminate);
	};

	u8 read_file_flags(bool bit_big_endian)
	{
		u8 flags = 0;
		read(&flags,sizeof(u8),1);
		return bit_big_endian ? flags : flip_byte(flags);
	};

	u8 read_format(bool bit_big_endian)
	{
		u8 format = 0;
		read(&format,sizeof(u8),1);
		bool reverse = bit_big_endian || (format & 1) != 0 && (format & 0b10000000) == 0;
		return reverse ? format : flip_byte(format);
	};

	void write_file_flags(u8 flags, bool bit_big_endian)
	{
		bool reverse = bit_big_endian;
		write_u8(reverse ? flags : flip_byte(flags));
	};

	void write_format(u8 format, bool bit_big_endian)
	{
		bool reverse = bit_big_endian || (format & 0b00000001) != 0;
		write_u8(reverse ? format : flip_byte(format));
	};

	bool& big_endian() {return m_big_endian;};

	void flip_bytes(u8* bytes, i32 length) const
	{
		switch(length)
		{
			case(1):
				break;
			case(2):
				std::swap(bytes[0],bytes[1]);
				break;
			case(4):
				std::swap(bytes[0],bytes[3]);
				std::swap(bytes[1],bytes[2]);
				break;
			case(8):
				std::swap(bytes[0],bytes[7]);
				std::swap(bytes[1],bytes[6]);
				std::swap(bytes[2],bytes[5]);
				std::swap(bytes[3],bytes[4]);
				break;
			default:
				throw std::runtime_error("flip_bytes(): legnth "+std::to_string(length)+"\n");
		}
	};

	i8 read_i8()
	{
		i8 r = 0;
		read(&r,sizeof(i8),1);
		return r;
	};

	//read int16, accounting for endianness
	i16 read_i16()
	{
		i16 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	//read int32, accounting for endianness
	i32 read_i32()
	{
		i32 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	//read int64, accounting for endianness
	i64 read_i64()
	{
		i64 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	u8 read_u8()
	{
		u8 r = 0;
		read(&r,sizeof(r),1);
		return r;
	};

	void read_u8(std::vector<u8*> ptrs)
	{
		for(i32 i = 0; i < ptrs.size(); i++)
			*ptrs[i] = read_u8();
	};

	//read uint16, accounting for endianness
	u16 read_u16()
	{
		u16 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	//read uint32, accounting for endianness
	u32 read_u32()
	{
		u32 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	//read uint64, accounting for endianness
	u64 read_u64()
	{
		u64 r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	//read fp32, accounting for endianness
	float read_f32()
	{
		float r = 0;
		read(&r,sizeof(r),1);
		if(m_big_endian)
			flip_bytes((u8*)&r,sizeof(r));
		return r;
	};

	void read_f32(std::vector<float*> ptrs)
	{
		for(i32 i = 0; i < ptrs.size(); i++)
			*ptrs[i] = read_f32();
	};

	vec2f read_v2f()
	{
		vec2f v;
		for(i32 i = 0; i < 2; i++)
			v[i] = read_f32();
		return v;
	};

	vec3f read_v3f()
	{
		vec3f v;
		for(i32 i = 0; i < 3; i++)
			v[i] = read_f32();
		return v;
	};

	vec4f read_v4f()
	{
		vec4f v;
		for(i32 i = 0; i < 4; i++)
			v[i] = read_f32();
		return v;
	};

	void write_i8(i8 i)
	{
		write(&i,sizeof(i),1);
	};

	//write int16, accounting for endianness
	void write_i16(i16 i)
	{
		if(m_big_endian)
			flip_bytes((u8*)&i,sizeof(i));
		write(&i,sizeof(i),1);
	};

	//write int32, accounting for endianness
	void write_i32(i32 i)
	{
		if(m_big_endian)
			flip_bytes((u8*)&i,sizeof(i));
		write(&i,sizeof(i),1);
	};

	//write int64, accounting for endianness
	void write_i64(i64 i)
	{
		if(m_big_endian)
			flip_bytes((u8*)&i,sizeof(i));
		write(&i,sizeof(i),1);
	};

	void write_u8(u8 u)
	{
		write(&u,sizeof(u8),1);
	};

	void write_u8(std::vector<u8> values)
	{
		for(i32 i = 0; i < values.size(); i++)
			write_u8(values[i]);
	};

	//write uint16, accounting for endianness
	void write_u16(u16 u)
	{
		if(m_big_endian)
			flip_bytes((u8*)&u,sizeof(u));
		write(&u,sizeof(u),1);
	};

	//write uint32, accounting for endianness
	void write_u32(u32 u)
	{
		if(m_big_endian)
			flip_bytes((u8*)&u,sizeof(u));
		write(&u,sizeof(u),1);
	};

	//write uint64, accounting for endianness
	void write_u64(u64 u)
	{
		if(m_big_endian)
			flip_bytes((u8*)&u,sizeof(u));
		write(&u,sizeof(u),1);
	};

	//write fp32, accounting for endianness
	void write_f32(float f)
	{
		if(m_big_endian)
			flip_bytes((u8*)&f,sizeof(f));
		write(&f,sizeof(f),1);
	};

	void write_f32(std::vector<float> values)
	{
		for(i32 i = 0; i < values.size(); i++)
			write_f32(values[i]);
	};

	void write_v2f(vec2f v)
	{
		for(i32 i = 0; i < 2; i++)
			write_f32(v[i]);
	};

	void write_v3f(vec3f v)
	{
		for(i32 i = 0; i < 3; i++)
			write_f32(v[i]);
	};

	void write_v4f(vec4f v)
	{
		for(i32 i = 0; i < 4; i++)
			write_f32(v[i]);
	};

	color_t read_rgba()
	{
		color_t c;
		c.r = read_u8();
		c.g = read_u8();
		c.b = read_u8();
		c.a = read_u8();
		return c;
	};

	color_t read_argb()
	{
		color_t c;
		c.a = read_u8();
		c.r = read_u8();
		c.g = read_u8();
		c.b = read_u8();
		return c;
	};

	color_t read_abgr()
	{
		color_t c;
		c.a = read_u8();
		c.b = read_u8();
		c.g = read_u8();
		c.r = read_u8();
		return c;
	};

	color_t read_bgra()
	{
		color_t c;
		c.b = read_u8();
		c.g = read_u8();
		c.r = read_u8();
		c.a = read_u8();
		return c;
	};

	void write_rgba(color_t color)
	{
		write_u8(color.r);
		write_u8(color.g);
		write_u8(color.b);
		write_u8(color.a);
	};

	void write_argb(color_t color)
	{
		write_u8(color.a);
		write_u8(color.r);
		write_u8(color.g);
		write_u8(color.b);
	};

	void write_abgr(color_t color)
	{
		write_u8(color.a);
		write_u8(color.b);
		write_u8(color.g);
		write_u8(color.r);
	};

	void write_bgra(color_t color)
	{
		write_u8(color.b);
		write_u8(color.g);
		write_u8(color.r);
		write_u8(color.a);
	};

	bool assert_i8(i8 x, i8* dst = nullptr)
	{
		i8 d = read_i8();
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};

	bool assert_i32(i32 x, i32* dst = nullptr)
	{
		i32 d = read_i32();
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};

	bool assert_i64(i64 x, i64* dst = nullptr)
	{
		i64 d = read_i64();
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};

	bool assert_u8(u8 x, u8* dst = nullptr)
	{
		u8 d = read_u8();
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};

	bool assert_any_u8(std::vector<u8> v, u8* dst = nullptr)
	{
		u8 d;
		read(&d,sizeof(u8),1);
		if(dst != nullptr)
			*dst = d;
		for(i32 i = 0; i < v.size(); i++)
			if(d == v[i])
				return true;
		std::string expected = {};
		for(i32 i = 0; i < v.size(); i++)
			expected += (std::to_string(v[i])+",");
		printf(
			"UMEM assert: Got %s, expected {%s}\n",
			std::to_string(d).c_str(),expected.c_str()
		);
		return false;
	};

	bool assert_u32(u32 x, u32* dst = nullptr)
	{
		u32 d = read_u32();
		if(dst != nullptr)
			*dst = d;
		if(d != x)
			printf(
				"UMEM assert: Got %s, expected %s\n",
				std::to_string(d).c_str(),
				std::to_string(x).c_str()
			);
		return d == x;
	};

	private:
	void reserve(std::string name, std::string type_name, i32 length)
	{
		std::string s = name + ":" + type_name;
		if(m_reservations.find(s) != m_reservations.end())
			throw std::runtime_error("UMEM: Found key "+s+"!\n");
		m_reservations.insert({s,m_pos});
		for(i32 i = 0; i < length; i++)
			write_u8(0xFE);
	};

	public:
	void reserve_i8(std::string name) {reserve(name,"i8",1);};
	
	void reserve_i16(std::string name) {reserve(name,"i16",2);};
	
	void reserve_i32(std::string name) {reserve(name,"i32",4);};
	
	void reserve_i64(std::string name) {reserve(name,"i64",8);};

	void reserve_u8(std::string name) {reserve(name,"u8",1);};
	
	void reserve_u16(std::string name) {reserve(name,"u16",2);};
	
	void reserve_u32(std::string name) {reserve(name,"u32",4);};
	
	void reserve_u64(std::string name) {reserve(name,"u64",8);};

	private:
	i64 fill(std::string name, std::string type_name)
	{
		std::string s = name + ":" + type_name;
		if(m_reservations.find(s) == m_reservations.end())
			throw std::runtime_error("UMEM: Failed to find key "+s+"!\n");
		i64 jump = m_reservations.at(s);
		m_reservations.erase(s);
		return jump;
	};

	public:
	void fill_i8(std::string name, i8 i) {step_in(fill(name,"i8"))->write(&i,sizeof(i),1); step_out();};
	
	void fill_i16(std::string name, i16 i) {step_in(fill(name,"i16"))->write(&i,sizeof(i),1); step_out();};
	
	void fill_i32(std::string name, i32 i) {step_in(fill(name,"i32"))->write(&i,sizeof(i),1); step_out();};
	
	void fill_i64(std::string name, i64 i) {step_in(fill(name,"i64"))->write(&i,sizeof(i),1); step_out();};
	
	void fill_u8(std::string name, u8 u) {step_in(fill(name,"u8"))->write(&u,sizeof(u),1); step_out();};
	
	void fill_u16(std::string name, u16 u) {step_in(fill(name,"u16"))->write(&u,sizeof(u),1); step_out();};
	
	void fill_u32(std::string name, u32 u) {step_in(fill(name,"u32"))->write(&u,sizeof(u),1); step_out();};
	
	void fill_u64(std::string name, u64 u) {step_in(fill(name,"u64"))->write(&u,sizeof(u),1); step_out();};

	void pad(i32 align)
	{
		while(m_pos % align > 0)
			write_u8(0);
	};

	i32 error() const
	{
		if(m_is_file)
			return ferror(m_file);
		return m_err;
	};
};

inline UMEM* uopen(const std::string &path, const std::string &mode)
{
	return new UMEM(path,mode);
};

inline UMEM* uopen(i64 bytes)
{
	return new UMEM(bytes);
};

inline void uclose(UMEM* mem)
{
	if(mem != nullptr)
		delete mem;
};

inline i64 usize(UMEM* mem)
{
	if(mem != nullptr)
		return mem->m_size;
	return -1;
};

inline i64 uread(void* dst, i64 size, i64 n, UMEM* mem)
{
	if(mem == nullptr || dst == nullptr)
		return 0;
	return mem->read(dst,size,n);
};

inline i64 uwrite(void* src, i64 size, i64 n, UMEM* mem)
{
	if(mem == nullptr || src == nullptr)
		return 0;
	return mem->write(src,size,n);
};

inline i64 useek(UMEM* mem, i64 offset, i32 whence)
{
	if(mem == nullptr)
		return -1;
	return mem->seek(offset,whence);
};

inline i64 utell(UMEM* mem)
{
	if(mem == nullptr)
		return -1;
	return mem->tell();
};