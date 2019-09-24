#pragma once


#include "LuaIntf/LuaIntf.h"
#include <vector>
#include <map>
#include <sstream>

using std::stringstream;

#define LUA_BINDING_METHOD_NAME _LUA_BindToLUA

#define LUA_BIND_METHOD_OVERLOAD(CLASS, FUNCTION_LUA_NAME, FUNCTION, RET_TYPE, ...) .addFunction(#FUNCTION_LUA_NAME, static_cast<RET_TYPE(CLASS::*)(__VA_ARGS__)>(&CLASS::FUNCTION))
#define LUA_BIND_METHOD(CLASS, FUNCTION) .addFunction(#FUNCTION, &CLASS::FUNCTION)
#define LUA_BIND_METHOD_ARGS(CLASS, FUNCTION, ...) .addFunction(#FUNCTION, &CLASS::FUNCTION, LUA_ARGS(__VA_ARGS__))
#define LUA_BIND_PROPERTY(CLASS, PROPERTY, GETTER_FUNC_NAME, SETTER_FUNC_NAME) .addProperty(#PROPERTY, &CLASS::GETTER_FUNC_NAME, &CLASS::SETTER_FUNC_NAME)
#define LUA_BIND_STATIC_METHOD(CLASS, FUNCTION) .addStaticFunction(#FUNCTION, &CLASS::FUNCTION)
//A tester
//#define LUA_BIND_STATIC_METHOD_OVERLOAD(CLASS, FUNCTION_LUA_NAME, FUNCTION, RET_TYPE, ...) .addStaticFunction(#FUNCTION_LUA_NAME, static_cast<RET_TYPE(CLASS::*)(__VA_ARGS__)>(&CLASS::FUNCTION))

#define LUA_BIND_CONSTRUCTOR_NOARGS .addConstructor(LUA_ARGS())
#define LUA_BIND_CONSTRUCTOR(...) .addConstructor(LUA_ARGS(__VA_ARGS__))
#define LUA_BIND_CONSTRUCTOR_SP_NOARGS(CLASS) .addConstructor(LUA_SP(std::shared_ptr<CLASS>), LUA_ARGS())
#define LUA_BIND_CONSTRUCTOR_SP(CLASS, ...) .addConstructor(LUA_SP(std::shared_ptr<CLASS>), LUA_ARGS(__VA_ARGS__))
//#define LUA_BING_FACTORY(...) .addFactory([](Color c, float s, float r, float i) {return std::shared_ptr<Material>(new Material(c, s, r, i)); })

#define LUA_INTERNAL_START_BIND_CLASS(CLASS) (Parent.template beginClass<CLASS>(#CLASS))
#define LUA_INTERNAL_START_BIND_CLASS_SUBCLASS_OF(CLASS, SUPER) Parent.template beginExtendClass<CLASS, SUPER>(#CLASS)

#define LUA_BEGIN_BIND_METHODS(CLASS) template <typename PARENT> static inline PARENT LUA_BINDING_METHOD_NAME (PARENT Parent) { return LUA_INTERNAL_START_BIND_CLASS(CLASS)
#define LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(CLASS, SUPER) template <typename PARENT> static inline PARENT LUA_BINDING_METHOD_NAME (PARENT Parent) { return LUA_INTERNAL_START_BIND_CLASS_SUBCLASS_OF(CLASS, SUPER)

#define LUA_BIND_CLASS(CLASS, PARENT) CLASS::LUA_BINDING_METHOD_NAME (PARENT)
#define LUA_END_BIND_METHODS .endClass(); }

//modules
#define LUA_START_BIND_MODULE_BASE(MODULE, LUA_CONTEXT) LuaIntf::LuaBinding(LUA_CONTEXT).beginModule(#MODULE)
#define LUA_START_BIND_MODULE(MODULE, PARENT) PARENT.beginModule(#MODULE)
#define LUA_END_BIND_MODULE(MODULE) MODULE.endModule()
//----------------------------------------------------------------------
//-----------------ENUMS------------------------------------------------
//----------------------------------------------------------------------

#define END_LINE '\n'

#define LUA_ENUM_FUNCTION_NAME _Lua_EnumToLUA

#define LUA_ENUM_BEGIN_WRITE_FUNTION(ENUM) inline stringstream& LUA_ENUM_FUNCTION_NAME (stringstream& fs) { fs << #ENUM << " = {}" << END_LINE
#define LUA_ENUM_WRITE_VALUE(ENUM, VALUE) << #ENUM << "." << #VALUE << " = " << static_cast<int>(Type::VALUE) << END_LINE
#define LUA_ENUM_END_WRITE_FUNCTION << END_LINE << END_LINE; return fs;} 

#define LUA_ENUM_WRITE_ENUM(ENUM, STREAM) ENUM::LUA_ENUM_FUNCTION_NAME(STREAM);

//----------------------------------------------------------------------
//----------------------------------------------------------------------
namespace LuaIntf
{
    LUA_USING_SHARED_PTR_TYPE(std::shared_ptr);
    LUA_USING_LIST_TYPE(std::vector)
    LUA_USING_MAP_TYPE(std::map)
}
