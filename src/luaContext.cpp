#include "luaContext.h"
#include "luaRegisterer.h"

#include <boost/filesystem.hpp>
#include <vector>

#include "sceneRenderer.h"

using std::vector;
using std::string;
using std::cerr;

using LuaIntf::LuaException;

using namespace boost::filesystem;

namespace FalconEye {

    LuaContext::LuaContext()
        : luaContext()
    {
        LUARegisterer::RegisterClasses(luaContext);
		LUARegisterer::GenerateEnums();
        //loadScripts();
    }

    LuaContext::LuaContext(const char* config_file)
        : luaContext()
    {
        LUARegisterer::RegisterClasses(luaContext);
		LUARegisterer::GenerateEnums();
        //loadScripts();
        configure(config_file);
    }

    LuaContext::~LuaContext()
    {}

    int LuaContext::configure(const char * config_file)
    {
        if (!runFile(config_file))
        {
            config_RunScripts();
            config_SetDefaultRenderOption();
            config_SetLuaPath();
        }
        else
        {
            cerr << "configuration file could not be read\n";
            return 1;
        }

        return 0;
    }

    int LuaContext::runFile(const char* file)
    {
        try
        {
            luaContext.doFile(file);
        }
        catch (const LuaException& e)
        {
            cerr << "Error while trying to run file " << file << '\n' << e.what() << '\n';
            return 1;
        }
        return 0;
    }

    int LuaContext::runFilesInDirectory(const char* dir)
    {
        path scriptsDirectory(dir);
        if (exists(scriptsDirectory))
        {
            if (is_directory(scriptsDirectory))
            {
                vector<string> filesToExecute;
                for (auto&& it = scriptsDirectory.begin(); it != scriptsDirectory.end(); ++it)
                {
                    path current = *it;
                    if (is_regular_file(current))
                    {
                        if (current.extension().string() == ".lua")
                        {
                            filesToExecute.push_back(current.string());
                        }
                    }
                }
                return runFiles(filesToExecute);
            }
        }

        return -1;
    }

    int LuaContext::runMainScript()
    {
        int ret = 0;
        LuaRef programConfigRef = getProgramConfigRef();
        if (programConfigRef.isTable())
        {
            string f = programConfigRef.get<string>("mainScript");
            return runFile(f.c_str());
        }
        else
        {
            cerr << "configuration file does not define variable \"programConfig\" or it is not a table\n";
            ret = 1;
        }
        return ret;
    }

    LuaRef LuaContext::getProgramConfigRef()
    {
        return LuaRef(luaContext.state(), "programConfig");
    }

    int LuaContext::config_RunScripts()
    {
        int ret = 0;
        vector<string> filesToRun;
        LuaRef programConfigRef = getProgramConfigRef();
        if (programConfigRef.isTable())
        {
            LuaRef scriptsTable = programConfigRef["scripts"];
            if (scriptsTable.isTable())
            {
                filesToRun = scriptsTable.toValue<vector<string>>();
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            cerr << "configuration file does not define variable \"programConfig\" or it is not a table\n";
            ret = 1;
        }

        if (runFiles(filesToRun) != 0)
        {
            cerr << "not all scripts of programConfig could be run\n";
            return 1;
        }
        return ret;
    }

    int LuaContext::config_SetDefaultRenderOption()
    {
        LuaRef programConfigRef = getProgramConfigRef();
        if (programConfigRef.isTable())
        {
            try
            {
                SceneRenderOption ro = programConfigRef.get<SceneRenderOption>("defaultRenderOptions");
                SceneRenderOption::defaultRenderOptions = ro;
            }
            catch (const LuaException& e)
            {
                cerr << "Error while trying to get value for \"defaultRenderOptions\" : " << e.what() << '\n';
                return 1;
            }
        }
        else
        {
            cerr << "configuration file does not define variable \"defaultRenderOptions\" or it is not a table\n";
        }
        return 0;
    }
    
    int LuaContext::config_SetLuaPath()
    {
        LuaRef programConfigRef = getProgramConfigRef();
        if (programConfigRef.isTable())
        {
            try
            {
                std::string lua_path = programConfigRef.get<std::string>("lua_path");
                if (lua_path.size() > 0)
                {
					luaContext.setGlobal("LUA_PATH", lua_path);
				}
            }
            catch (const LuaException& e)
            {
                cerr << "Error while trying to get value for \"lua_path\" : " << e.what() << '\n';
                return 1;
            }
        }
        return 0;
    }

    int LuaContext::runFiles(const vector<string>& files)
    {
        int ret = 0;
        for (auto&& f : files)
        {
            ret += runFile(f.c_str());
        }
        return ret;
    }

} // end namespace FalconEye
