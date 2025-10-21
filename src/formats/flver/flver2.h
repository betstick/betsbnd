#pragma once
#include "../file.h"
#include "flver.h"

class flver2_t : public file_t
{
	struct header_t
	{
		bool big_endian = false;
		i32 version = 0;
		vec3f bounding_box_min;
		vec3f bounding_box_max;
		bool unicode = false;
		u8  unk4a = 0;
		u32 unk4c = 0;
		u8  unk5c = 0;
		u8  unk5d = 0;
		u32 unk68 = 0;

		header_t()
		{
			big_endian = false;
			version = 0x20014;
			unicode = true;
		};

		i32 read(UMEM* mem);

		i32 write(UMEM* mem);
	};

	struct texture_t
	{
		std::string type;
		std::string path;
		vec2f scale;
		u8 unk10;
		bool unk11;
		float unk14;
		float unk18;
		float unk1c;
	};

	struct faceset_t
	{
		u32 flags;
		bool tri_strip;
		bool cull_backfaces;
		u16 unk06;
		std::vector<i32> indices = {};
	};

	struct vertex_buffer_t;

	struct mesh_t
	{
		u8 dynamic;
		i32 material_index;
		i32 default_bone_index;
		std::vector<i32> bone_indices = {};
		std::vector<faceset_t*> facesets = {};
		std::vector<vertex_buffer_t*> vertex_buffers = {};
		std::vector<flver::vertex_t> vertices = {};

		bounding_box_t bounding_box;

		std::vector<i32> faceset_indices = {};
		std::vector<i32> vertex_buffer_indices = {};
	};

	struct material_t
	{
		std::string name;
		std::string mtd;
		i32 falgs;
		std::vector<texture_t*> textures = {};
		i32 gx_index;
		i32 unk18;
		i32 texture_index;
		i32 texture_count;
	};

	struct vertex_buffer_t
	{
		i32 layout_index;
		i32 vertex_size;
		i32 buffer_index;
		i32 vertex_count;
		i32 buffer_offset;
	};

	struct buffer_layout_t
	{
		i32 size;
	};

	struct gx_item_t
	{
		std::string id = "0";
		i32 unk04 = 100;
		u8* data = nullptr;
		i32 length = 0;

		void read(UMEM* mem, i32 header_version)
		{
			if(header_version <= 0x20010)
			{
				id = std::to_string(mem->read_i32());
			}
			else
			{
				id.resize(4);
				mem->read(id.data(),1,4);
			}

			unk04 = mem->read_i32();
			length = mem->read_i32();
			if(data != nullptr)
				delete data;
			data = new u8[length-0xC];
			mem->read(data,sizeof(u8),length-0xC);
		};

		void write(UMEM* mem, i32 header_version)
		{
			if(header_version <= 0x20010)
			{
				mem->write_i32(std::stoi(id));
			}
			else
			{
				mem->write(id.data(),sizeof(char),4);
			}

			mem->write_i32(unk04);
			mem->write_i32(length+0xC);
			mem->write(data,sizeof(u8),length-0xC);
		};
	};

	struct gx_list_t
	{
		std::vector<sp<gx_item_t>> gx_items = {};
		i32 terminator_id = INT32_MAX;
		i32 terminator_length;

		static sp<gx_list_t> create(UMEM* mem, i32 header_version)
		{
			sp<gx_list_t> gx = sp<gx_list_t>();
			gx->read(mem,header_version);
			return gx;
		};

		void read(UMEM* mem, i32 header_version)
		{
			if(header_version < 0x20010)
			{
				sp<gx_item_t> gx = sp<gx_item_t>();
				gx->read(mem,header_version);
				gx_items.push_back(gx);
			}
			else
			{
				i32 id;
				while(true)
				{
					id = mem->read_i32();
					if(id != INT32_MAX && id != -1)
					{
						sp<gx_item_t> gx = sp<gx_item_t>();
						gx->read(mem,header_version);
						gx_items.push_back(gx);
					}
					else
					{
						break;
					}
				}

				terminator_id = mem->read_i32();
				mem->assert_i32(100);
			}
		};
	};

	header_t m_header;
	std::vector<sp<flver::dummy_t>> m_dummies = {};
	std::vector<sp<flver::bone_t>> m_bones = {};
	std::vector<sp<material_t>> m_materials = {};

	public:
	flver2_t();

	i32 read(UMEM* mem);

	i32 write(UMEM* mem);
};