local libDir = "dependencies/"

local SLD2_image = libDir .. "SDL2_image-2.0.1/"
local SDL2 = libDir .. "SDL2-2.0.8/"
local glew = libDir .. "glew-2.1.0/"
local lua = libDir .."lua/"
local boost = "D:/Developpement/boost_1_66_0/"

local libs_dir_release_x64 = { SLD2_image .. "lib/x64/", SDL2 .. "lib/x64",  glew .. "lib/Release/x64/", lua,  boost .. "stage/lib" }
local libs_dir_release_x86 = { SLD2_image .. "lib/x86/", SDL2 .. "lib/x86",  glew .. "lib/Release/Win32/", lua,  boost .. "stage/lib"}
local libs_includedirs = {SLD2_image .. "include", SDL2 .. "include", glew .. "include", lua .. "include", boost}
local global_defines = { "BOOST_FILESYSTEM_NO_DEPRECATED" }

solution "gKit2light"
	configurations { "debug", "release" }
	platforms { "x64", "x32"  }

	includedirs { ".", "src/gKit", "src" }
	includedirs (libs_includedirs)
	
	configuration "debug"
		targetdir "bin/debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "release"
		targetdir "bin/release"
--~ 		defines { "NDEBUG" }
--~ 		defines { "GK_RELEASE" }
		flags { "Optimize" }

	configuration "linux"
		includedirs ("../lua/include/")
		libdirs { "../lua/" }
		buildoptions { "-mtune=native" }
		buildoptions { "-std=c++20" }
		buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
		buildoptions { "-flto"}
		linkoptions { "-flto"}
		linkoptions { "-pthread"}
		links { "GLEW", "SDL2", "SDL2_image", "GL", "pthread", "lua", "boost_filesystem", "boost_system"} -- TODO ajouter png aux autres config
		buildoptions { "-fopenmp" }
		linkoptions { "-fopenmp" }
		defines { "NB_THREADS=16" }
        defines (global_defines)

	configuration { "windows" }
		defines { "WIN32", "NVWIDGETS_EXPORTS", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
		defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((
        defines (global_defines)

	configuration { "windows", "codeblocks", "x32" }
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		includedirs (libs_includedirs)
		libdirs (libs_dir_release_x86)
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32", "lua" }

	configuration { "windows", "vs2012", "x64" }
		includedirs { "extern/visual2012/include" }
		libdirs { "extern/visual2012/lib" }
		includedirs (libs_includedirs)
		libdirs (libs_dir_release_x64)
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image", "lua" }

	configuration { "windows", "vs2013", "x64" }
		includedirs { "extern/visual2013/include" }
		libdirs { "extern/visual2013/lib" }
		includedirs (libs_includedirs)
		libdirs (libs_dir_release_x64)
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image", "lua" }

	configuration { "windows", "vs2015", "x64" }
		includedirs { "extern/visual2015/include" }
		libdirs { "extern/visual2015/lib" }
		includedirs (libs_includedirs)
		libdirs (libs_dir_release_x64)
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image", "lua" }

	configuration { "windows", "vs2019", "x64" }
		includedirs { "extern/visual2015/include" }
		libdirs { "extern/visual2015/lib" }
		includedirs (libs_includedirs)
		libdirs (libs_dir_release_x64)
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image", "lua" }

	configuration "macosx"
		local frameworks= "-F /Library/Frameworks/"
		defines { "GK_MACOS" }
		buildoptions { frameworks }
		linkoptions { frameworks .. " -framework OpenGL -framework SDL2 -framework SDL2_image" }


 -- description des fichiers communs
local src = "src/"
local subDirs = { "gKit", "LuaInterface", "LuaIntf", "Raycasting", "Rendering", "Threading", "Utils", "37C3"}

local function generateProjectFiles(src, subDirs)
    local ret = {}
	for i,v in ipairs(subDirs) do 
		table.insert(ret, src .. v .. "/*.cpp")
		table.insert(ret, src .. v .. "/*.h")
		table.insert(ret, src .. v .. "/*.hpp")
	end

	return ret
end

local gkit_files = { "src/gKit/*.cpp", "src/gKit/*.h" }
local FalconEye_files = { "src/*.cpp", "src/*.h", "src/*.hpp"}
local Threading_files = { "src/Threading/*.cpp", "src/Threading/*.h", "src/Threading/*.hpp"}

project("FalconEye")
	language "c++"
	kind "ConsoleApp"
	targetdir "bin"
	files (gkit_files)
	files (FalconEye_files)
	files (Threading_files)
	files (generateProjectFiles(src, subDirs))
