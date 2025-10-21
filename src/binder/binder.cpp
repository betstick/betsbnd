#include "binder.h"
#include "bnd3.h"
#include "bnd4.h"
#include <stdexcept>

binder_t* binder_t::read(UMEM* mem)
{
	std::string magic = "0000";
	mem->step_in(0);
	mem->read(magic.data(),sizeof(char),4);
	mem->step_out();
	if(same_str(magic,"BND3"))
		return bnd3_t::read(mem);
	else if(same_str(magic,"BND4"))
		return bnd4_t::read(mem);
	else
		throw std::runtime_error("binder_t::read() magic: "+magic+"\n");
};