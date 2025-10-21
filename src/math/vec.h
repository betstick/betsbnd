#pragma once
#include "../common.h"

struct vec2f
{
	float x, y;

	float operator[](i32 i) const {return ((float*)this)[i];};
	float& operator[](i32 i) {return ((float*)this)[i];};
};

struct vec3f
{
	float x, y, z;

	vec3f operator+(float f) const {return vec3f(x+f,y+f,z+f);};
	vec3f operator-(float f) const {return vec3f(x-f,y-f,z-f);};
	vec3f operator*(float f) const {return vec3f(x*f,y*f,z*f);};
	vec3f operator/(float f) const {return vec3f(x/f,y/f,z/f);};

	float operator[](i32 i) const {return ((float*)this)[i];};
	float& operator[](i32 i) {return ((float*)this)[i];};
};

struct vec4f
{
	float x, y, z, w;

	float operator[](i32 i) const {return ((float*)this)[i];};
	float& operator[](i32 i) {return ((float*)this)[i];};
};

struct vec2i
{
	i32 x, y;

	i32 operator[](i32 i) const {return ((i32*)this)[i];};
	i32& operator[](i32 i) {return ((i32*)this)[i];};
};

struct vec3i
{
	i32 x, y, z;

	i32 operator[](i32 i) const {return ((i32*)this)[i];};
	i32& operator[](i32 i) {return ((i32*)this)[i];};
};

struct vec4i
{
	i32 x, y, z, w;

	i32 operator[](i32 i) const {return ((i32*)this)[i];};
	i32& operator[](i32 i) {return ((i32*)this)[i];};
};