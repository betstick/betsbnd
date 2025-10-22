#include "common.h"

int main(int argv, const char** argc)
{
	printf("Test: DSR\n");

	if(argv < 2)
	{
		printf("No path to DSR given. Exitting.\n");
		return 0;
	}

	printf("Path: %s\n",argc[1]);
	std::string path = "";
	i32 i = 0;
	while(true)
	{
		if(argc[1][i] == '\0')
			break;
		path.push_back(argc[1][i]);
		i++;
	}

	path += "chr/c5370.chrbnd.dcx";

	UMEM* f = uopen(path,"rb");
	

	dcx_t* dcx = dcx_t::open(f);
	
	UMEM* b = uopen(99999);
	dcx->decompress(b);

	b->seek(0,SEEK_SET);
	bnd3_t* bnd3 = bnd3_t::read(b);

	printf("File count: %lu\n",bnd3->get_headers().size());
};