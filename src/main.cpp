#include "luaContext.h"

using std::cout;
using std::endl;
using std::string;
using namespace FalconEye;

int main( int argc, char **argv )
{

    FalconEye::LuaContext luaContext("data/lua/config.lua");
    luaContext.runMainScript();
    
    return 0;
}