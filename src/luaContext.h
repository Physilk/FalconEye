#pragma once

//#include "LuaIntf/LuaRef.h"
//#include "LuaIntf/LuaState.h"
#include "LuaIntf/LuaIntf.h"

#include <string>
#include <iostream>
#include <vector>

//#include <boost/any.hpp>

using std::string;
using std::vector;
//using std::cerr;

using LuaIntf::LuaRef;

namespace FalconEye {

    class LuaContext
    {
    public:
        LuaContext();
        LuaContext(const char* config_file);
        LuaContext(const LuaContext&) = delete;
        ~LuaContext();

        int configure(const char* config_file);
        int runFile(const char* file);
        int runFiles(const vector<string>& files);
        int runFilesInDirectory(const char* dir);
        int runMainScript();

    private:
        LuaIntf::LuaContext luaContext;
        LuaRef getProgramConfigRef();

        int config_RunScripts();
        int config_SetDefaultRenderOption();
        int config_SetLuaPath();
    };

} // end namespace FalconEye
