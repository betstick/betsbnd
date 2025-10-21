#include "flver2.h"

flver2_t::flver2_t() : file_t(FILE_FLVER2) {};

i32 flver2_t::read(UMEM* mem)
{
	mem->big_endian() = false;

	i32 data_offset = mem->read_i32();
	mem->read_i32();
	i32 dum_count  = mem->read_i32();	
	i32 mat_count  = mem->read_i32();
	i32 bone_count = mem->read_i32();
	i32 mesh_count = mem->read_i32();
	i32 vbuf_count = mem->read_i32();

	m_header.bounding_box_min = mem->read_v3f();
	m_header.bounding_box_max = mem->read_v3f();

	mem->read_i32(); //face count (no blur or degen meshes)
	mem->read_i32(); //total face count

	i32 vert_idx_size = mem->read_i32();
	
	m_header.unicode = mem->read_u8();
	m_header.unk4a = mem->read_u8();
	if(!mem->assert_u8(0))
		return -1;

	m_header.unk4c = mem->read_i32();

	i32 face_set_count = mem->read_i32();
	i32 buff_layout_count = mem->read_i32();
	i32 tex_count = mem->read_i32();

	m_header.unk5c = mem->read_u8();
	m_header.unk5d = mem->read_u8();
	for(i32 i = 0; i < 2; i++)
		if(!mem->assert_u8(0))
			return 0;
	for(i32 i = 0; i < 2; i++)
		if(!mem->assert_u32(0))
			return 0;

	m_header.unk68 = mem->read_i32();

	for(i32 i = 0; i < 5; i++)
		if(!mem->assert_u32(0))
			return 0;

	for(i32 i = 0; i < dum_count; i++)
		m_dummies.push_back(flver::dummy_t::create(mem,m_header.version));

	for(i32 i = 0; i < mat_count; i++)
		m_materials.push_back(material_t::create(mem,m_header,gx_lists,gx_list_indices));

	return 0;
};