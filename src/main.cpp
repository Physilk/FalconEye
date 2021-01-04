#include "LuaInterface/luaContext.h"

using std::cout;
using std::endl;
using std::string;
using namespace FalconEye;

int main( int argc, char **argv )
{
	try
	{
		FalconEye::LuaContext luaContext("data/lua/config.lua");
		luaContext.runMainScript();
	}
	catch (std::exception* e)
	{
		std::cerr << e->what();
	}
    
    return 0;
}
