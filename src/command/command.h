#include "../common.h"
#include "../util/strings.h"
#include "../binder/binder.h"

inline void process(std::string args)
{
	std::string binder_path;
	std::string output_path;
	std::string cmd;

	if(same_str(cmd,"list")) //list all files in the binder
	{

	}
	if(same_str(cmd,"cd"))
	{

	}
	else if(same_str(cmd,"extract")) //extract a file from the binder
	{

	}
	else if(same_str(cmd,"insert")) //insert a file into the binder
	{

	}
	else if(same_str(cmd,"move")) //move a file around in the binder
	{

	}
	else if(same_str(cmd,"copy")) //copy a file in the binder
	{

	}
	else if(same_str(cmd,"delete")) //remove a file from the binder
	{

	}
	else if(same_str(cmd,"commit")) //commit changes back to disk
	{

	}
	else if(same_str(cmd,"discard")) //discard changes to the binder
	{

	}
	else if(same_str(cmd,"help")) //print list of commands or info about specific command
	{

	}
};

struct navigator_t
{
	std::string root_dir;
	std::vector<binder_t*> binders = {};
};