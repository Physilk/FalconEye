#pragma once


#include "LuaIntf/LuaIntf.h"

#define LUA_BINDING_METHOD_NAME _LUA_BindToLUA

#define LUA_BIND_METHOD_OVERLOAD(CLASS, FUNCTION_LUA_NAME, FUNCTION, RET_TYPE, ...) .addFunction(#FUNCTION_LUA_NAME, static_cast<RET_TYPE(CLASS::*)(__VA_ARGS__)>(&CLASS::FUNCTION))
#define LUA_BIND_METHOD(CLASS, FUNCTION) .addFunction(#FUNCTION, &CLASS::FUNCTION)
#define LUA_BIND_PROPERTY(CLASS, PROPERTY, GETTER_FUNC_NAME, SETTER_FUNC_NAME) .addProperty(#PROPERTY, &CLASS::GETTER_FUNC_NAME, &CLASS::SETTER_FUNC_NAME)
#define LUA_BIND_STATIC_METHOD(CLASS, FUNCTION) .addStaticFunction(#FUNCTION, &CLASS::FUNCTION)
//A tester
//#define LUA_BIND_STATIC_METHOD_OVERLOAD(CLASS, FUNCTION_LUA_NAME, FUNCTION, RET_TYPE, ...) .addStaticFunction(#FUNCTION_LUA_NAME, static_cast<RET_TYPE(CLASS::*)(__VA_ARGS__)>(&CLASS::FUNCTION))

#define LUA_BIND_CONSTRUCTOR_NOARGS .addConstructor(LUA_ARGS())
#define LUA_BIND_CONSTRUCTOR(...) .addConstructor(LUA_ARGS(__VA_ARGS__))
#define LUA_BIND_CONSTRUCTOR_SP_NOARGS(CLASS) .addConstructor(LUA_SP(std::shared_ptr<CLASS>), LUA_ARGS())
#define LUA_BIND_CONSTRUCTOR_SP(CLASS, ...) .addConstructor(LUA_SP(std::shared_ptr<CLASS>), LUA_ARGS(__VA_ARGS__))
//#define LUA_BING_FACTORY(...) .addFactory([](Color c, float s, float r, float i) {return std::shared_ptr<Material>(new Material(c, s, r, i)); })

#define LUA_INTERNAL_START_BIND_CLASS(CLASS) LuaIntf::LuaBinding(L).beginClass<CLASS>(#CLASS)
#define LUA_INTERNAL_START_BIND_CLASS_SUBCLASS_OF(CLASS, SUPER) LuaIntf::LuaBinding(L).beginExtendClass<CLASS, SUPER>(#CLASS)

#define LUA_BEGIN_BIND_METHODS(CLASS) static inline void LUA_BINDING_METHOD_NAME (lua_State *L) { LUA_INTERNAL_START_BIND_CLASS(CLASS)
#define LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(CLASS, SUPER) static inline void LUA_BINDING_METHOD_NAME (lua_State *L) { LUA_INTERNAL_START_BIND_CLASS_SUBCLASS_OF(CLASS, SUPER)

#define LUA_BIND_CLASS(CLASS, LUA_STATE) CLASS::LUA_BINDING_METHOD_NAME (LUA_STATE);
#define LUA_END_BIND_METHODS .endClass(); }

//.addStaticFunction("go_home", &Web::go_home)
namespace LuaIntf
{
    LUA_USING_SHARED_PTR_TYPE(std::shared_ptr);
}