#pragma once
#include "../util/umem.h"

enum file_type_e
{
	FILE_DRB     = 1 <<  0,
	FILE_EMEVD   = 1 <<  2,
	FILE_FFXDLSE = 1 <<  3,
	FILE_FLVER1  = 1 <<  4,
	FILE_FLVER2  = 1 <<  5,
	FILE_FLVER   = FILE_FLVER1 | FILE_FLVER2,
	FILE_MQB     = 1 <<  6,
	FILE_MSB     = 1 <<  7,
	FILE_PARAM   = 1 <<  8,
	FILE_TAE3    = 1 <<  9,
	FILE_TPF     = 1 << 10,
};

class file_t
{
	protected:
	i32 m_file_type = 0;

	file_t(i32 file_type_) : m_file_type(file_type_) {};

	public:
	static file_t* read_any(UMEM* mem);

	//read from UMEM handle
	virtual i32 read(UMEM* mem) = 0;

	//write to UMEM handle
	virtual i32 write(UMEM* mem) = 0;
};