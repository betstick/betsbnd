#pragma once
#include "../common.h"
#include <iconv.h>
#include <stdexcept>

inline bool same_str(std::string a, std::string b)
{
	if(a.size() != b.size())
		return false;
	for(i32 i = 0; i < a.size(); i++)
		if(a[i] != b[i])
			return false;
	return true;
};

namespace str_conv
{
	inline std::string __conv(std::string src, std::string src_fmt, std::string dst_fmt)
	{
		i32 len = src.length();
		char p_src[4096];
		char p_dst[4096];
		memset(p_src,0,sizeof(char)*4096);
		memset(p_dst,0,sizeof(char)*4096);
		for(i32 i = 0; i < len; i++)
			p_src[i] = src[i];
		size_t n_src = len;
		size_t n_dst = len * 4;

		char* ps = p_src;
		char* pd = p_dst;

		iconv_t icd = iconv_open(src_fmt.c_str(),dst_fmt.c_str());
		if(icd == (iconv_t)-1)
			throw std::runtime_error("Failed to open iconv!\n");

		if(iconv(icd,&ps,&n_src,&pd,&n_dst) == (size_t)-1)
			throw std::runtime_error("Failed to run iconv!\n");

		p_dst[len*4+1-n_dst] = 0;
		//printf("p_dst: %s\n",p_dst);

		std::string out;
		i32 i = 0;
		while(true)
		{
			char c = p_dst[i];
			if(c == '\0')
				break;
			out.push_back(c);
			i++;
			if(i > len * 4)
				throw std::runtime_error("__conv(): "+src_fmt+" to "+dst_fmt+" overflow!\n");
		}

		iconv_close(icd);
		return out;
	};

	inline std::string utf16_to_utf8(std::string str, bool big_endian)
	{
		std::string fmt = "UTF-16";
		if(big_endian)
			fmt = "UTF-16BE";
		return __conv(str,fmt,"UTF-8");
	};

	inline std::string shift_jis_to_utf8(std::string str)
	{
		return __conv(str,"Shift_JIS","UTF-8");
	};

	inline std::string utf8_to_utf16(std::string str, bool big_endian)
	{
		std::string fmt = "UTF-16";
		if(big_endian)
			fmt = "UTF-16BE";
		return __conv(str,"UTF-8",fmt);
	};

	inline std::string utf8_to_shift_jis(std::string str)
	{
		return __conv(str,"UTF-8","Shift_JIS");
	};
};
