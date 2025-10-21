#pragma once
#include "../../math/vec.h"
#include "../../util/umem.h"
#include "../../util/types.h"
#include <algorithm>
#include <cstdint>
#include <queue>
#include <stdexcept>

namespace flver
{
	inline float read_u8_norm(UMEM* mem)
	{
		return (float)(mem->read_u8() - 127) / 127.0f;
	};

	inline vec3f read_u8_norm_xyz(UMEM* mem)
	{
		vec3f n;
		for(i32 i = 0; i < 3; i++)
			n[i] = read_u8_norm(mem);
		return n;
	};

	inline vec4f read_u8_norm_xyzw(UMEM* mem)
	{
		vec4f n;
		for(i32 i = 0; i < 4; i++)
			n[i] = read_u8_norm(mem);
		return n;
	};

	inline float read_i8_norm(UMEM* mem)
	{
		return (float)mem->read_u8() / 127.0f;
	};

	inline vec3f read_i8_norm_zyx(UMEM* mem)
	{
		vec3f n;
		n.z = read_i8_norm(mem);
		n.y = read_i8_norm(mem);
		n.x = read_i8_norm(mem);
		return n;
	};

	inline float read_i16_norm(UMEM* mem)
	{
		return (float)mem->read_i16() / 32767.0f;
	};

	inline vec3f read_i16_norm_xyz(UMEM* mem)
	{
		vec3f n;
		for(i32 i = 0; i < 3; i++)
			n[i] = read_i16_norm(mem);
		return n;
	};

	inline vec4f read_i16_norm_xyzw(UMEM* mem)
	{
		vec4f n;
		for(i32 i = 0; i < 4; i++)
			n[i] = read_i16_norm(mem);
		return n;
	};

	inline float read_u16_norm(UMEM* mem)
	{
		return (float)(mem->read_u16() - 32767) / 32767.0f;
	};

	inline vec3f read_u16_norm_xyz(UMEM* mem)
	{
		vec3f n;
		for(i32 i = 0; i < 3; i++)
			n[i] = read_u16_norm(mem);
		return n;
	};

	inline void write_u8_norm(UMEM* mem, float f)
	{
		mem->write_u8(std::clamp<float>(f*127.0f+127.0f,0.0f,127.0f));
	};

	inline void write_u8_norm_xyz(UMEM* mem, vec3f v)
	{
		for(i32 i = 0; i < 3; i++)
			write_u8_norm(mem,v[i]);
	};

	inline void write_u8_norm_xyzw(UMEM* mem, vec4f v)
	{
		for(i32 i = 0; i < 4; i++)
			write_u8_norm(mem,v[i]);
	};

	inline void write_i8_norm(UMEM* mem, float f)
	{
		mem->write_i8(std::clamp<float>(f*127.0f,INT8_MIN,INT8_MAX));
	};

	inline void write_i8_norm_zyx(UMEM* mem, vec3f v)
	{
		write_i8_norm(mem,v.z);
		write_i8_norm(mem,v.y);
		write_i8_norm(mem,v.x);
	};

	inline void write_i16_norm(UMEM* mem, float f)
	{
		mem->write_i16(std::clamp<float>(f*32767.0f,INT16_MIN,INT16_MAX));
	};

	inline void write_i16_norm_xyz(UMEM* mem, vec3f v)
	{
		for(i32 i = 0; i < 3; i++)
			write_i16_norm(mem,v[i]);
	};

	inline void write_i16_norm_xyzw(UMEM* mem, vec4f v)
	{
		for(i32 i = 0; i < 4; i++)
			write_i16_norm(mem,v[i]);
	};

	inline void write_u16_norm(UMEM* mem, float f)
	{
		mem->write_u16(std::clamp<float>(f*32767.0f+32767.0f,0,UINT16_MAX));
	};

	inline void write_u16_norm_xyz(UMEM* mem, vec3f v)
	{
		for(i32 i = 0; i < 3; i++)
			write_u16_norm(mem,v[i]);
	};

	struct dummy_t
	{
		vec3f position;
		vec3f forward;
		vec3f upward;
		i16 ref_id;
		i16 parent_bone_index;
		i16 attach_bone_index;
		color_t color;
		bool flag1;
		bool use_upward_vector;
		i32 unk30;
		i32 unk34;

		static sp<dummy_t> create(UMEM* mem, i32 version)
		{
			sp<dummy_t> dum = sp<dummy_t>();
			dum->read(mem,version);
			return dum;
		};

		void read(UMEM* mem, i32 version)
		{
			position = mem->read_v3f();
			if(version == 0x20010)
				color = mem->read_bgra();
			else
				color = mem->read_argb();
			forward = mem->read_v3f();
			ref_id = mem->read_i16();
			parent_bone_index = mem->read_i16();
			upward = mem->read_v3f();
			attach_bone_index = mem->read_i16();
			flag1 = mem->read_u8();
			use_upward_vector = mem->read_u8();
			unk30 = mem->read_i32();
			unk34 = mem->read_i32();
			mem->assert_i32(0);
			mem->assert_i32(0);
		};

		void write(UMEM* mem, i32 version)
		{
			mem->write_v3f(position);
			if(version == 0x20010)
				mem->write_bgra(color);
			else
				mem->write_argb(color);
			mem->write_v3f(forward);
			mem->write_i16(ref_id);
			mem->write_i16(parent_bone_index);
			mem->write_v3f(upward);
			mem->write_i16(attach_bone_index);
			mem->write_u8(flag1);
			mem->write_u8(use_upward_vector);
			mem->write_i32(unk30);
			mem->write_i32(unk34);
			mem->write_i32(0);
			mem->write_i32(0);
		};
	};

	struct bone_t
	{
		std::string name;
		i16 parent_index;
		i16 child_index;
		i16 next_sibling_index;
		i16 prev_sibling_index;
		vec3f translation;
		vec3f rotation;
		vec3f scale;
		vec3f bounding_box_min;
		vec3f bounding_box_max;
		i32 unk3c;

		static sp<bone_t> create(UMEM* mem, bool unicode)
		{
			sp<bone_t> bone = sp<bone_t>();
			bone->read(mem,unicode);
			return bone;
		};

		void read(UMEM* mem, bool unicode)
		{
			translation = mem->read_v3f();
			i32 name_offset = mem->read_i32();
			rotation = mem->read_v3f();
			parent_index = mem->read_i16();
			child_index = mem->read_i16();
			scale = mem->read_v3f();
			next_sibling_index = mem->read_i16();
			prev_sibling_index = mem->read_i16();
			bounding_box_min = mem->read_v3f();
			unk3c = mem->read_i32();
			bounding_box_max = mem->read_v3f();
			mem->assert_u8(0x34);
			mem->assert_u8(0x00);
		};

		void write(UMEM* mem, i32 index)
		{
			mem->write_v3f(translation);
			mem->reserve_i32("bone_name_offset"+std::to_string(index));
			mem->write_v3f(rotation);
			mem->write_i16(parent_index);
			mem->write_i16(child_index);
			mem->write_v3f(scale);
			mem->write_i16(next_sibling_index);
			mem->write_i16(prev_sibling_index);
			mem->write_v3f(bounding_box_min);
			mem->write_i32(unk3c);
			mem->write_v3f(bounding_box_max);
			mem->write_u8(0x34);
			mem->write_u8(0x00);
		};

		void write_strings(UMEM* mem, bool unicde, i32 index)
		{
			mem->fill_i32("bone_name_offset"+std::to_string(index),mem->m_pos);
			if(unicde)
				mem->write_utf16(name,true);
			else
				mem->write_shift_jis(name,true);
		};
	};

	enum layout_semantic_e
	{
		LOS_POSITION = 0,
		LOS_BONE_WEIGHTS = 1,
		LOS_BONE_INDICES = 2,
		LOS_NORMAL = 3,
		LOS_UV = 5,
		LOS_TANGENT = 6,
		LOS_BITANGENT = 7,
		LOS_VERTEX_COLOR = 10
	};

	enum layout_type_e
	{
		LOT_FLOAT2 = 0x01,
		LOT_FLOAT3 = 0x02,
		LOT_FLOAT4 = 0x03,
		LOT_BYTE4A = 0x10,
		LOT_BYTE4B = 0x11,
		LOT_SHORT2_TO_FLOAT2 = 0x11,
		LOT_BYTE4C = 0x12,
		LOT_UV = 0x13,
		LOT_UV_PAIR = 0x15,
		LOT_SHORT_BONE_INDICES = 0x18,
		LOT_SHORT4_TO_FLOAT4A = 0x1a,
		LOT_SHORT4_TO_FLOAT4B = 0x2e,
		LOT_BYTE4E = 0x2f,
		LOT_EDGE_COMPRESSED = 0xf0,
	};

	struct layout_member_t
	{
		i32 unk00;
		u32 type;
		u32 semantic;
		i32 index;
		i32 size;

		void read(UMEM* mem, i32 struct_offset)
		{
			unk00 = mem->read_i32();
			mem->assert_i32(struct_offset);
			type = mem->read_u32();
			semantic = mem->read_u32();
			index = mem->read_i32();
			//TODO: calculate size
		};

		void write(UMEM* mem, i32 struct_offset)
		{
			mem->write_i32(unk00);
			mem->write_i32(struct_offset);
			mem->write_u32(type);
			mem->write_u32(semantic);
			mem->write_i32(index);
		};
	};

	struct vertex_bone_weights_t
	{
		float a, b, c, d;

		float operator[](i32 i) const {return ((float*)this)[i];};
		float& operator[](i32 i) {return ((float*)this)[i];};
	};

	struct vertex_bone_indices_t
	{
		i32 a, b, c, d;

		i32 operator[](i32 i) const {return ((i32*)this)[i];};
		i32& operator[](i32 i) {return ((i32*)this)[i];};
	};

	struct vertex_color_t
	{
		float a, r, g, b;

		void read_rgba_f32(UMEM* mem)
		{
			mem->read_f32({&r,&g,&b,&a});
		};

		void read_argb_f32(UMEM* mem)
		{
			mem->read_f32({&a,&r,&g,&b});
		};

		void read_rgba_u8(UMEM* mem)
		{
			r = (float)mem->read_u8() / 255.0f;
			g = (float)mem->read_u8() / 255.0f;
			b = (float)mem->read_u8() / 255.0f;
			a = (float)mem->read_u8() / 255.0f;
		};

		void read_argb_u8(UMEM* mem)
		{
			a = (float)mem->read_u8() / 255.0f;
			r = (float)mem->read_u8() / 255.0f;
			g = (float)mem->read_u8() / 255.0f;
			b = (float)mem->read_u8() / 255.0f;
		};

		void write_rgba_f32(UMEM* mem) const
		{
			mem->write_f32({r,g,b,a});
		};
		
		void write_argb_f32(UMEM* mem) const
		{
			mem->write_f32({a,r,g,b});
		};

		void write_rgba_u8(UMEM* mem) const
		{
			mem->write_u8({u8(r*255.f),u8(g*255.f),u8(b*255.f),u8(a*255.f)});
		};
		
		void write_argb_u8(UMEM* mem) const
		{
			mem->write_u8({u8(a*255.f),u8(r*255.f),u8(g*255.f),u8(b*255.f)});
		};
	};

	struct vertex_t
	{
		vec3f position;
		vertex_bone_weights_t bone_weights;
		vertex_bone_indices_t bone_indices;
		vec3f normal;
		i32 normal_w;
		std::vector<vec3f> uvs = {};
		std::vector<vec4f> tangents = {};
		vec4f bitangent;
		std::vector<vertex_color_t> colors;

		std::queue<vec3f> uv_queue;
		std::queue<vec4f> tangent_queue;
		std::queue<vertex_color_t> color_queue;

		void read(UMEM* mem, std::vector<layout_member_t*> layout, float uv_factor)
		{
			for(i32 i = 0; i < layout.size(); i++)
			{
				layout_member_t* member = layout[i];
				if(member->semantic == layout_semantic_e::LOS_POSITION)
				{
					if(member->type == layout_type_e::LOT_FLOAT3)
					{
						position = mem->read_v3f();
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						position = mem->read_v3f();
						mem->read_f32();
					}
					else if(member->type == layout_type_e::LOT_EDGE_COMPRESSED)
					{
						printf("Error: Edge compressed vertex!\n");
						position = {0.0f,0.0f,0.0f};
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BONE_WEIGHTS)
				{
					if(member->type == layout_type_e::LOT_BYTE4A)
					{
						for(i32 i = 0; i < 4; i++)
							bone_weights[i] = (float)mem->read_i8() / 127.0f;
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						for(i32 i = 0; i < 4; i++)
							bone_weights[i] = (float)mem->read_u8() / 255.0f;
					}
					else if(member->type == layout_type_e::LOT_UV_PAIR)
					{
						for(i32 i = 0; i < 4; i++)
							bone_weights[i] = (float)mem->read_i16() / 32767.0f;
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						for(i32 i = 0; i < 4; i++)
							bone_weights[i] = (float)mem->read_i16() / 32767.0f;
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BONE_INDICES)
				{
					if(member->type == layout_type_e::LOT_BYTE4B)
					{
						for(i32 i = 0; i < 4; i++)
							bone_indices[i] = mem->read_u8();
					}
					else if(member->type == layout_type_e::LOT_SHORT_BONE_INDICES)
					{
						for(i32 i = 0; i < 4; i++)
							bone_indices[i] = mem->read_u16();
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						for(i32 i = 0; i < 4; i++)
							bone_indices[i] = mem->read_u8();
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_NORMAL)
				{
					if(member->type == layout_type_e::LOT_FLOAT3)
					{
						normal = mem->read_v3f();
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						normal = mem->read_v3f();
						float w = mem->read_f32();
						normal_w = (i32)w;
						if(w != normal_w)
							throw std::runtime_error("Bad W!\n");
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						normal = read_u8_norm_xyz(mem);
						normal_w = mem->read_u8();
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						normal = read_u8_norm_xyz(mem);
						normal_w = mem->read_u8();
					}
					else if(member->type == layout_type_e::LOT_SHORT2_TO_FLOAT2)
					{
						normal_w = mem->read_u8();
						normal = read_i8_norm_zyx(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						normal = read_u8_norm_xyz(mem);
						normal_w = mem->read_u8();
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						normal = read_i16_norm_xyz(mem);
						normal_w = mem->read_i16();
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4B)
					{
						normal = read_u16_norm_xyz(mem);
						normal_w = mem->read_i16();
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						normal = read_u8_norm_xyz(mem);
						normal_w = mem->read_u8();
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_UV)
				{
					if(member->type == layout_type_e::LOT_FLOAT2)
					{
						uvs.push_back(vec3f(mem->read_f32(),mem->read_f32(),0.0f));
					}
					else if(member->type == layout_type_e::LOT_FLOAT3)
					{
						uvs.push_back(mem->read_v3f());
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						uvs.push_back(vec3f(mem->read_f32(),mem->read_f32(),0.0f));
						uvs.push_back(vec3f(mem->read_f32(),mem->read_f32(),0.0f));
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_SHORT2_TO_FLOAT2)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_UV)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_UV_PAIR)
					{
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
						uvs.push_back(vec3f(mem->read_i16(),mem->read_i16(),0.0f)/uv_factor);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4B)
					{
						uvs.push_back(vec3f(
							mem->read_i16(),mem->read_i16(),mem->read_i16()
						)/uv_factor);
						mem->read_i16();
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_TANGENT)
				{
					if(member->type == layout_type_e::LOT_FLOAT4)
					{
						tangents.push_back(mem->read_v4f());
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						tangents.push_back(read_u8_norm_xyzw(mem));
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						tangents.push_back(read_u8_norm_xyzw(mem));
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						tangents.push_back(read_u8_norm_xyzw(mem));
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						tangents.push_back(read_i16_norm_xyzw(mem));
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						tangents.push_back(read_u8_norm_xyzw(mem));
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BITANGENT)
				{
					if(member->type == layout_type_e::LOT_BYTE4A)
					{
						bitangent = read_u8_norm_xyzw(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						bitangent = read_u8_norm_xyzw(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						bitangent = read_u8_norm_xyzw(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						bitangent = read_u8_norm_xyzw(mem);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_VERTEX_COLOR)
				{
					if(member->type == layout_type_e::LOT_FLOAT4)
					{
						vertex_color_t v; v.read_rgba_f32(mem);
						colors.push_back(v);
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						vertex_color_t v; v.read_rgba_u8(mem);
						colors.push_back(v);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						vertex_color_t v; v.read_rgba_u8(mem);
						colors.push_back(v);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else
				{
					printf("member semantic: %x\n",member->semantic);
					printf("member type: %x\n",member->type);
					throw std::runtime_error("flver::vertex_t unknown member semantic!\n");
				}
			}
		};

		void write(UMEM* mem, std::vector<layout_member_t*> layout, float uv_factor)
		{
			for(i32 i = 0; i < layout.size(); i++)
			{
				layout_member_t* member = layout[i];
				if(member->semantic == layout_semantic_e::LOS_POSITION)
				{
					if(member->type == layout_type_e::LOT_FLOAT3)
					{
						mem->write_v3f(position);
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						mem->write_v3f(position);
						mem->write_f32(0.0f);
					}
					else if(member->type == layout_type_e::LOT_EDGE_COMPRESSED)
					{
						printf("Error: Edge compressed vertex!\n");
						position = {0.0f,0.0f,0.0f};
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BONE_WEIGHTS)
				{
					if(member->type == layout_type_e::LOT_BYTE4A)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_i8(bone_weights[i]*127.0f);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_u8(bone_weights[i]*255.0f);
					}
					else if(member->type == layout_type_e::LOT_UV_PAIR)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_i16(bone_weights[i]*32767.0f);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_i16(bone_weights[i]*32767.0f);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BONE_INDICES)
				{
					if(member->type == layout_type_e::LOT_BYTE4B)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_u8(bone_indices[i]);
					}
					else if(member->type == layout_type_e::LOT_SHORT_BONE_INDICES)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_u16(bone_indices[i]);
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						for(i32 i = 0; i < 4; i++)
							mem->write_u8(bone_indices[i]);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_NORMAL)
				{
					if(member->type == layout_type_e::LOT_FLOAT3)
					{
						mem->write_v3f(normal);
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						mem->write_v3f(normal);
						mem->write_f32(normal_w);
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						write_u8_norm_xyz(mem,normal);
						mem->write_u8(normal_w);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						write_u8_norm_xyz(mem,normal);
						mem->write_u8(normal_w);
					}
					else if(member->type == layout_type_e::LOT_SHORT2_TO_FLOAT2)
					{
						mem->write_u8(normal_w);
						write_i8_norm_zyx(mem,normal);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						write_u8_norm_xyz(mem,normal);
						mem->write_u8(normal_w);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						write_i16_norm_xyz(mem,normal);
						mem->write_i16(normal_w);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4B)
					{
						write_u16_norm_xyz(mem,normal);
						mem->write_i16(normal_w);
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						write_u8_norm_xyz(mem,normal);
						mem->write_u8(normal_w);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_UV)
				{
					vec3f uv = uv_queue.front() * uv_factor;
					uv_queue.pop();

					if(member->type == layout_type_e::LOT_FLOAT2)
					{
						mem->write_f32(uv.x);
						mem->write_f32(uv.y);
					}
					else if(member->type == layout_type_e::LOT_FLOAT3)
					{
						mem->write_v3f(uv);
					}
					else if(member->type == layout_type_e::LOT_FLOAT4)
					{
						mem->write_f32(uv.x);
						mem->write_f32(uv.y);

						uv = uv_queue.front() * uv_factor;
						uv_queue.pop();
						mem->write_f32(uv.x);
						mem->write_f32(uv.y);
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_SHORT2_TO_FLOAT2)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_UV)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_UV_PAIR)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);

						uv = uv_queue.front() * uv_factor;
						uv_queue.pop();
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4B)
					{
						mem->write_i16(uv.x);
						mem->write_i16(uv.y);
						mem->write_i16(uv.z);
						mem->write_i16(0);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_TANGENT)
				{
					vec4f tangent = tangent_queue.front();
					tangent_queue.pop();

					if(member->type == layout_type_e::LOT_FLOAT4)
					{
						mem->write_v4f(tangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						write_u8_norm_xyzw(mem,tangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						write_u8_norm_xyzw(mem,tangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						write_u8_norm_xyzw(mem,tangent);
					}
					else if(member->type == layout_type_e::LOT_SHORT4_TO_FLOAT4A)
					{
						write_i16_norm_xyzw(mem,tangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						write_u8_norm_xyzw(mem,tangent);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_BITANGENT)
				{
					if(member->type == layout_type_e::LOT_BYTE4A)
					{
						write_u8_norm_xyzw(mem,bitangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4B)
					{
						write_u8_norm_xyzw(mem,bitangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						write_u8_norm_xyzw(mem,bitangent);
					}
					else if(member->type == layout_type_e::LOT_BYTE4E)
					{
						write_u8_norm_xyzw(mem,bitangent);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else if(member->semantic == layout_semantic_e::LOS_VERTEX_COLOR)
				{
					vertex_color_t color = color_queue.front();
					color_queue.pop();

					if(member->type == layout_type_e::LOT_FLOAT4)
					{
						color.write_rgba_f32(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4A)
					{
						color.read_rgba_u8(mem);
					}
					else if(member->type == layout_type_e::LOT_BYTE4C)
					{
						color.read_rgba_u8(mem);
					}
					else
					{
						printf("member semantic: %x\n",member->semantic);
						printf("member type: %x\n",member->type);
						throw std::runtime_error("flver::vertex_t unknown member type!\n");
					}
				}
				else
				{
					printf("member semantic: %x\n",member->semantic);
					printf("member type: %x\n",member->type);
					throw std::runtime_error("flver::vertex_t unknown member semantic!\n");
				}
			}
		};

		void prep_write()
		{
			for(i32 i = 0; i < colors.size(); i++)
				color_queue.push(colors[i]);
			for(i32 i = 0; i < uvs.size(); i++)
				uv_queue.push(uvs[i]);
			for(i32 i = 0; i < tangents.size(); i++)
				tangent_queue.push(tangents[i]);
		};
	};
};